"""End-to-end checks for the console game."""

import os
from pathlib import Path
import re
import subprocess
import tempfile
import unittest


ROOT = Path(__file__).resolve().parents[1]
STUDENTS = {
    "Guan Yu", "Zhang Fei", "Liu Bei", "Ma Chao", "Huang Zhong",
    "Zhao Yun", "Jiang Wei", "Zhuge Liang", "Xu Shu", "Jiang Yi",
}


class GameTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.build_dir = tempfile.TemporaryDirectory()
        cls.executable = Path(cls.build_dir.name) / (
            "road-of-life.exe" if os.name == "nt" else "road-of-life"
        )
        subprocess.run(
            [os.environ.get("CXX", "g++"), "-std=c++11", "-Wall", "-Wextra",
             "-pedantic", "src/The Road of Life.cpp", "-o", str(cls.executable)],
            cwd=ROOT, check=True,
        )

    @classmethod
    def tearDownClass(cls):
        cls.build_dir.cleanup()

    def play(self, *answers):
        completed = subprocess.run(
            [str(self.executable)], input="\n".join(map(str, answers)) + "\n",
            text=True, encoding="utf-8", capture_output=True, timeout=10, check=True,
        )
        return completed.stdout + completed.stderr

    def graded_names(self, output):
        results = output.split("Homework results:\n", 1)[1]
        results = results.split("After five years", 1)[0]
        return re.findall(r"^(.+?)\s+\|\s+(?:Excellent|Good|Pass|Fail)$", results, re.M)

    def test_invalid_choices_and_closed_input(self):
        output = self.play("hello", 99, 1, 4)
        self.assertIn("Please enter a whole number from 1 to 3", output)
        self.assertIn("Age: 18", output)
        self.assertNotIn("View your attributes", output)
        self.assertIn("3. Clear the conversation", output)
        self.assertIn("4. End your journey", output)
        self.assertIn("Game over.", output)

        output = self.play(1, 5, 4)
        self.assertIn("Please enter a whole number from 1 to 4", output)
        self.assertIn("Game over.", output)

        output = self.play(1, 1, 1, 2, 1, 1, 1, "Alex Male")
        self.assertIn("Input closed. Goodbye.", output)

    def test_each_character_can_complete_undergraduate_studies(self):
        for role, name in [(1, "Chenxi"), (2, "Haochen"), (3, "Yuyao")]:
            with self.subTest(name=name):
                output = self.play(role, 1, 1, 4)
                self.assertIn("earned your bachelor's degree", output)
                self.assertIn(f"Name: {name}", output)
                self.assertIn("Age: 22", output)
                self.assertRegex(output, r"Age: 22  Wisdom: \d+")

    def test_automatic_grading_records_all_initial_submissions(self):
        output = self.play(2, 1, 3, 1, 1, 2, 4)
        names = self.graded_names(output)
        self.assertEqual(len(names), 10)
        self.assertEqual(set(names), STUDENTS)
        self.assertIn("Age: 27", output)

    def test_individual_grading_can_switch_to_automatic(self):
        output = self.play(1, 1, 1, 1, 1, 1, 5, 4)
        names = self.graded_names(output)
        self.assertTrue(STUDENTS.issubset(names))
        self.assertIn(len(names), (10, 11))
        self.assertEqual(len(names), len(set(names)))

    def test_company_management_and_cancel_dismissal(self):
        output = self.play(
            1, 1, 1, 2, 1,
            1, 2, "NewA Male 30 1000", "NewB Female 31 2000", 1,
            3, 4, 3, 5, "LinFeng", 500, 1, 2, "XiaoYan", 0, 3, 6, 4,
        )
        self.assertIn("Salary increased.", output)
        self.assertIn("LinFeng | Male | 25 | 3 | 5500", output)
        self.assertIn("Dismiss XiaoYan?", output)
        self.assertIn("Dismissal canceled", output)
        self.assertIn("XiaoYan | Male | 35 | 10 | 10000", output)
        self.assertIn("Game over.", output)

    def test_consecutive_retirements(self):
        output = self.play(
            1, 1, 1, 2, 1, 1, 2,
            "ElderA Male 59 1000", "ElderB Female 59 2000", 1, 6,
            2, 1, 2, 1, 3, 6, 4,
        )
        self.assertIn("ElderA has reached retirement age and left the company.", output)
        self.assertIn("ElderB has reached retirement age and left the company.", output)

    def test_staff_capacity_does_not_terminate_the_game(self):
        answers = [1, 1, 1, 2, 1]
        for batch in range(5):
            answers.extend([1, 5])
            answers.extend(f"Staff{batch}_{i} Male 25 3000" for i in range(5))
            answers.append(1)
        answers.extend([1, 3, 6, 4])
        output = self.play(*answers)
        self.assertIn("The staff list is full, so you can't hire anyone else.", output)
        self.assertIn("Staff4_4", output)
        self.assertIn("Game over.", output)

    def test_second_work_period_requires_postgraduate_study(self):
        output = self.play(1, 1, 1, 2, 1, 6, 2, 1, 2, 1, 3, 6, 4)
        self.assertIn("It's time to begin the postgraduate studies", output)
        self.assertIn("LinFeng | Male | 31 | 9 | 5000", output)
        self.assertIn("Age: 29", output)

    def test_work_before_study_has_clear_guidance(self):
        output = self.play(1, 2, 4)
        self.assertIn("You'll need to complete your bachelor's degree before you can start working.", output)

    def test_comma_separated_hire_and_numeric_validation(self):
        output = self.play(
            1, 1, 1, 2, 1, 1, 1, "A, Male, 17, 20, 20x, 1,000,001, 20,000", 1, 3, 6, 4,
        )
        self.assertIn("Please enter a whole number from 18 to 59", output)
        self.assertIn("Please enter a whole number from 0 to 1000000", output)
        self.assertIn("A | Male | 20 | 0 | 20000", output)

    def test_cancel_batch_can_keep_discard_or_continue(self):
        prefix = [1, 1, 1, 2, 1, 1, 2, "Alice Female 25 5000", "cancel"]
        kept = self.play(*prefix, 1, 3, 6, 4)
        self.assertIn("Hired 1 employee:", kept)
        self.assertIn("Alice | Female | 25 | 0 | 5000", kept)

        discarded = self.play(*prefix, 2, 3, 6, 4)
        self.assertIn("Hiring canceled. No employees were hired.", discarded)
        self.assertNotIn("Alice | Female", discarded)

        continued = self.play(*prefix, 3, "Bob Male 29 8000", 1, 3, 6, 4)
        self.assertIn("Hired 2 employees:", continued)
        self.assertIn("Alice | Female | 25 | 0 | 5000", continued)
        self.assertIn("Bob | Male | 29 | 0 | 8000", continued)

    def test_cancel_salary_and_sort(self):
        output = self.play(1, 1, 1, 2, 1, 5, "LinFeng", 500, 0, 4, 0, 3, 6, 4)
        self.assertIn("Salary change canceled.", output)
        self.assertIn("Sorting canceled.", output)
        self.assertIn("LinFeng | Male | 25 | 3 | 5000", output)

    def test_salary_range_and_retry(self):
        output = self.play(1, 1, 1, 2, 1, 5, "LinFeng", "-5,001", "995,001", "-5,000", 1, 3, 6, 4)
        self.assertIn("Please enter a whole number from -5000 to 995000", output)
        self.assertIn("LinFeng | Male | 25 | 3 | 0", output)

    def test_cancel_hire_with_no_employees_can_still_finish_work(self):
        answers = [1, 1, 1, 2, 1]
        for name in ["LinFeng", "XiaoYan", "LinJing", "JiangWan", "LeYun"]:
            answers.extend([2, name, 1])
        answers.extend([1, 0, 3, 6, 4])
        output = self.play(*answers)
        self.assertIn("Hiring canceled. No employees were hired.", output)
        self.assertIn("Game over.", output)

    def test_status_notifications_and_new_game_reset(self):
        harness = Path(self.build_dir.name) / ("game-observer.exe" if os.name == "nt" else "game-observer")
        subprocess.run(
            [os.environ.get("CXX", "g++"), "-std=c++11", "-Wall", "-Wextra", "-pedantic",
             "-DROAD_OF_LIFE_GUI", "src/The Road of Life.cpp", "tests/game_observer_test.cpp",
             "-o", str(harness)], cwd=ROOT, check=True,
        )
        subprocess.run([str(harness)], cwd=ROOT, check=True, timeout=10)

    def test_staff_form_validation_and_atomic_operations(self):
        harness = Path(self.build_dir.name) / ("staff-logic.exe" if os.name == "nt" else "staff-logic")
        subprocess.run(
            [os.environ.get("CXX", "g++"), "-std=c++11", "-Wall", "-Wextra", "-pedantic",
             "-DROAD_OF_LIFE_GUI", "src/The Road of Life.cpp", "tests/staff_logic_test.cpp",
             "-o", str(harness)], cwd=ROOT, check=True,
        )
        subprocess.run([str(harness)], cwd=ROOT, check=True, timeout=10)

    @unittest.skipUnless(os.name == "nt", "The chat bridge uses the Windows API")
    def test_chat_bridge_and_interrupted_input(self):
        harness = Path(self.build_dir.name) / "chat-bridge.exe"
        subprocess.run(
            [os.environ.get("CXX", "g++"), "-std=c++11", "-Wall", "-Wextra", "-pedantic",
             "-DROAD_OF_LIFE_GUI", "src/The Road of Life.cpp", "tests/chat_bridge_test.cpp", "src/staff_dialog_win32.cpp",
             "-pthread", "-static", "-static-libgcc", "-static-libstdc++",
             "-o", str(harness), "-lgdi32", "-luser32"], cwd=ROOT, check=True,
        )
        subprocess.run([str(harness)], cwd=ROOT, check=True, timeout=10)

    @unittest.skipUnless(os.name == "nt", "The staff dialogs use the Windows API")
    def test_staff_dialog_layout_and_inline_validation(self):
        harness = Path(self.build_dir.name) / "staff-dialog.exe"
        subprocess.run(
            [os.environ.get("CXX", "g++"), "-std=c++11", "-Wall", "-Wextra", "-pedantic",
             "-DROAD_OF_LIFE_GUI", "src/The Road of Life.cpp", "tests/staff_dialog_test.cpp",
             "-static", "-static-libgcc", "-static-libstdc++",
             "-o", str(harness), "-lgdi32", "-luser32"], cwd=ROOT, check=True,
        )
        subprocess.run([str(harness)], cwd=ROOT, check=True, timeout=10)


if __name__ == "__main__":
    unittest.main()
