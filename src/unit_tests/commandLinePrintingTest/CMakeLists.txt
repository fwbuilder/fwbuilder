add_test(NAME commandLinePrintingTest COMMAND commandLinePrintingTest)

file(COPY commandLinePrintingTest test.fwb DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

