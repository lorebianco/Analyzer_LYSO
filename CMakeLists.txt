cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(Analyzer_LYSO)

### where to find the libraries
#set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/lib")

# Impostazione dei percorsi per ROOT
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
find_package(ROOT REQUIRED)
include(${ROOT_USE_FILE})

# Includi le directory dei file di intestazione
include_directories(${ROOT_INCLUDE_DIRS})
include_directories(${PROJECT_SOURCE_DIR}/include)

# Includi le directory delle librerie
link_directories(${ROOT_LIBRARY_DIR})

# Trova tutti i file sorgente
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

# Trova i file di macro e li copia nella directory binaria
file(GLOB MACRO_FILES ${PROJECT_SOURCE_DIR}/macros/*.mac)
file(COPY ${MACRO_FILES} DESTINATION ${PROJECT_BINARY_DIR})


# Aggiungi l'eseguibile analyzer_lyso
add_executable(analyzer_lyso analyzer_lyso.cc ${sources} ${headers})

# Collega le librerie ROOT
target_link_libraries(analyzer_lyso ${ROOT_LIBRARIES})


#Attach dictionaries to the executable. First, tell it where to look for headers required by the dictionaries:
#target_include_directories(myapp PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
# Then generate dictionaries and add them as a dependency of the executable (via the MODULE parameter):
ROOT_GENERATE_DICTIONARY(analyzer_dict ${headers} MODULE analyzer_lyso LINKDEF LinkDef.h)

add_library(analyzer SHARED ${sources} analyzer_dict.cxx)
target_link_libraries(analyzer ${ROOT_LIBRARIES})



# Se vuoi aggiungere un target custom
add_custom_target(Analyzer_LYSO DEPENDS analyzer_lyso)




# Aggiungi l'eseguibile analyzerMT
#add_executable(analyzer_lyso_MT analyzer_lyso_MT.cc)

# Definisci il target personalizzato per la generazione di entrambi gli eseguibili
#add_custom_target(Analyzer_LYSO ALL DEPENDS analyzer analyzerMT)