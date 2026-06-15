# Exam Prep Prompts

This folder contains reusable prompts for the NMPDE written exam.

- `exam-theory-prompt.md`: use it with Codex on an exam PDF to generate minimal theory answers and a printable/PDF helper.
- `exam-practise-prompt.md`: use it with Codex when you have an explicit exam trace and need a code submission folder.

## Standard Exam Folder Layout

After you have created/opened the exam folder yourself and put the PDF inside it, use the same output layout every time:

```text
sample-exam-try-N/
  Exam.pdf
  exam-theory/
    theory_answers.md
    quick_sheet.md
    theory_answers_overleaf.tex
    README_THEORY.md
  exam-practise/
    CMakeLists.txt
    README_SUBMISSION.md
    COMMANDS.md
    mesh/
    scripts/
    src/
    output/
```

Avoid alternate names such as `theory-submission-helper`, `practice-submission`, `submission-CODE`, `submission-THEORY`, `submission-exam`, or `practise-submission` unless you are maintaining an old folder.

## Main Rules

Codex should solve only the explicit exam trace you paste into the conversation. This avoids wasting time on unrelated material and keeps the generated submission folder aligned with what must be uploaded.

For code submissions, Codex should create the code and the run commands only. Moving the folder elsewhere is manual and outside the prompt. The standard build flow from inside the exam folder is:

```bash
cd exam-practise
gmsh -2 mesh/name.geo -o mesh/name.msh
cmake -S . -B build
cmake --build build -j$(nproc)
cd build
./name_of_executable
```

Do not run `cmake ..` from inside `exam-practise/`. That command is valid only from `exam-practise/build/`.
