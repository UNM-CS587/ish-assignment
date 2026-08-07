# Compile the PDF report
# 1. Locate the latexmk compiler tool. This is not REQUIRED: without a TeX
# distribution you can still configure the project, build ish, and run the
# test suite. You just can't build the report.
find_program(LATEXMK_EXECUTABLE latexmk)

if(NOT LATEXMK_EXECUTABLE)
    message(STATUS "latexmk not found; skipping the generate_pdf target. "
                   "Install a TeX distribution (see docs/DEVELOPMENT.md) to build report.pdf.")
    return()
endif()

# 2. Define source and output variables
set(LATEX_MAIN_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/report.tex")
set(LATEX_OUTPUT_PDF  "${CMAKE_CURRENT_BINARY_DIR}/report.pdf")

# 3. Use add_custom_command to compile only when source files change
add_custom_command(
    OUTPUT "${LATEX_OUTPUT_PDF}"
    COMMAND ${LATEXMK_EXECUTABLE} -pdf -interaction=nonstopmode -output-directory=${CMAKE_CURRENT_BINARY_DIR} "${LATEX_MAIN_SOURCE}"
    DEPENDS "${LATEX_MAIN_SOURCE}"
    COMMENT "Compiling LaTeX document with latexmk..."
    VERBATIM
)

# 4. Expose the command as a buildable CMake target. Deliberately not an ALL
# target: latexmk exits nonzero on a LaTeX error even under
# -interaction=nonstopmode, and a report that doesn't typeset must not fail
# the build that the code test cases depend on. Build it with
# `cmake --build build --target generate_pdf`.
add_custom_target(generate_pdf DEPENDS "${LATEX_OUTPUT_PDF}")
