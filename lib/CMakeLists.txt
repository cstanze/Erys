add_subdirectory(Parse)

newQuarkTarget(
  erysc

  BINARY
  Erys.cpp

  LINK_LIBS
  ErysParse
)
