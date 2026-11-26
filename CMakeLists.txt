# Projeto ESP-IDF da Merging Unit (Modulo 1a)
cmake_minimum_required(VERSION 3.22)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# test_host e compilado a parte (cmake -S test_host), nao faz parte do firmware
project(mu_firmware)
