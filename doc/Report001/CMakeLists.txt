project(Report001)

find_package(LATEX)

if(NOT PDFLATEX_COMPILER)
  message("pdflatex compiler was not found. Please pass to advanced mode and provide its full path")
else(NOT PDFLATEX_COMPILER)


find_program(FIG2DEV_EXECUTABLE NAMES fig2dev)


#
# These are source files needed for the report generation.
#
set(REPORT_SRCS
  algorithmic.sty
  algorithm.sty
  amssymb.sty
  fancyhdr.sty
  floatflt.sty
  fncychap.sty
  InsightArticle.cls
  InsightJournal.bib
  InsightJournal.ist
  InsightJournal.sty
  PrincipalComponentsAnalysis.tex
  CaudateFirstPrincipalComponentOnMomentum.jpg
  times.sty
  picins.sty
  )

#
# This adds a custom command for each source file in REPORT_SRCS
# that copies the file from the source directory to the binary
# directory where the pdf will be generated.
#
foreach(SOURCE_FILE ${REPORT_SRCS})
   add_custom_command(
     OUTPUT   ${Report001_BINARY_DIR}/${SOURCE_FILE}
     DEPENDS  ${Report001_SOURCE_DIR}/${SOURCE_FILE}
     COMMAND ${CMAKE_COMMAND} -E copy_if_different 
        ${Report001_SOURCE_DIR}/${SOURCE_FILE} 
        ${Report001_BINARY_DIR}/${SOURCE_FILE}
    )
   set(COPY_RESULTS ${COPY_RESULTS} ${Report001_BINARY_DIR}/${SOURCE_FILE})
endforeach(SOURCE_FILE ${REPORT_SRCS})

file(GLOB XFIG_FIGURES *.fig)

foreach(image ${XFIG_FIGURES})
  get_filename_component(Imagebase ${image} NAME)
  get_filename_component(ImageBase ${Imagebase} NAME_WE)
  add_custom_command( 
    OUTPUT    ${PROJECT_BINARY_DIR}/${ImageBase}.pdf
    DEPENDS   ${PROJECT_BINARY_DIR}/${ImageBase}.fig
    COMMAND   ${FIG2DEV_EXECUTABLE} 
      -L pdf ${PROJECT_BINARY_DIR}/${ImageBase}.fig ${PROJECT_BINARY_DIR}/${ImageBase}.pdf
    )
  set(EPS_DEPS ${EPS_DEPS} ${PROJECT_BINARY_DIR}/${ImageBase}.pdf)
endforeach(image)


set(REPORT_ELEMENTS 
  ${EPS_DEPS}
  )

#
# This adds a custom target that generates Report001.pdf
# This target depends on the list of copied files created
# with the custom command above and the Plots target.
#
add_custom_target(ReportIJ ALL 
   COMMAND ${PDFLATEX_COMPILER} 
        ${Report001_SOURCE_DIR}/PrincipalComponentsAnalysis.tex 
        -output-directory ${Report001_BINARY_DIR}
   COMMAND ${PDFLATEX_COMPILER} 
        ${Report001_SOURCE_DIR}/PrincipalComponentsAnalysis.tex 
        -output-directory ${Report001_BINARY_DIR}
   DEPENDS ${COPY_RESULTS} ${REPORT_ELEMENTS} 
   WORKING_DIRECTORY ${Report001_BINARY_DIR}
   )
 
endif(NOT PDFLATEX_COMPILER)
