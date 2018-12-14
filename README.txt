Assingment: Project3 (Code Generator)
Authors; Caleb, Greg, and Matt 

Description:
	This is the finalportion of the semester long project to make a scheme compiler. This project will take in a .ss scheme file
	and analyze the file for lexical correctness, syntactical correctness, and generate c++ code equivalent to the scheme code
	in the .ss file. The resulting file is able to be compiled using c++11 and the program can be run by typing "make" which will
	compile the program into an exacuteable P3.out. P3.out can be run using ./P3.out Some_Test_File.ss on the command line.
	The resulting c++ file from this will be name Some_Test_File.cpp where Some_Test_File is your files name and can be run by manually
	entering "g++ -std=c++11 Some_Test_File.cpp" on the command line.
	Note: We had to make a small edit the Object.cpp/.h to include the Correct file to compile we will include out Object class
	in the project submission.
What works:
     Correct c++ equivalent code should be made for a syntactically correct .ss file. The resulting .cpp file from a bad input
     will just be garbage and will most likely not be able to run.
What does not work:
     Exit Code Generation on bad input.

