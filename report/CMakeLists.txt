# Compile the PDF report
# 1. Locate the latexmk compiler tool
find_program(LATEXMK_EXECUTABLE latexmk REQUIRED)

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

# 4. Expose the command as a buildable CMake target
add_custom_target(generate_pdf ALL DEPENDS "${LATEX_OUTPUT_PDF}")
