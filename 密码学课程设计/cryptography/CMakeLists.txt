cmake_minimum_required(VERSION 3.15)
project(cryptography)

set(CMAKE_CXX_STANDARD 14)

add_executable(cryptography main.cpp)
add_executable(RSAParameterCalculation RSAParameterCalculation/RSAParameterCalculation.c)
add_executable(SPN SPN/SPN.c)
add_executable(SPNNEXT SPN/SPNNext.cpp)
add_executable(LinearAnalysis LinearAnalysis/LinearAnalysis.cpp)
add_executable(DifferentialAnalysis DifferentialAnalysis/DifferentialAnalysis.cpp)
add_executable(SPNEnhancement SPNEnhancement/SPNEnhancement.cpp)
add_executable(ModDupSquare ModDupSquare/ModDupSquare.cpp)
add_executable(ChineseSurplusTheory ChineseSurplusTheory/ChineseSurplusTheory.cpp)
add_executable(PKCS7 PKCS7/PKCS7.cpp)
add_executable(RainbowTable RainbowTable/RainbowTable.cpp)





