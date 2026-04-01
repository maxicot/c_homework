import subprocess

cases = [
    (
        "case1.csv",
        """\
+===========================+====================+
| Test field 1              | Test field 2       |
+===========================+====================+
| 28888880000000000888.5550 | 123a               |
+---------------------------+--------------------+
|                         6 |                3.0 |
+---------------------------+--------------------+
| other text                |  3.000000072394705 |
+---------------------------+--------------------+\n""",
    ),
    (
        "case2.csv",
        """\
+===================+==============+
| Test field 1      | Test field 2 |
+===================+==============+
| test              |          123 |
+-------------------+--------------+
| long string test! |         28.7 |
+-------------------+--------------+
| other text        |            3 |
+-------------------+--------------+\n""",
    ),
]

for filename, expected in cases:
    output = f"./tests/{filename}.output.txt"
    csv_pp = subprocess.run(
        [f"./csv_pp ./tests/{filename} {output}"],
        shell=True,
        capture_output=True,
        text=True,
    )

    with open(output, "r") as file:
        assert file.read() == expected
