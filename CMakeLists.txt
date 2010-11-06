cmake_minimum_required (VERSION 2.8)
project (TinyMocks)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

include_directories(include
    Tests/Helpers
)

add_executable (TinyMocksTests 
	main.cpp
	Tests/TestTinyMocks.cpp
	Tests/TestMockRepository.cpp
	Tests/Helpers/ComplexArgument.cpp
	Tests/Helpers/TestMock.cpp
)
