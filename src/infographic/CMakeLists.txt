
##########################
# Infographic
##########################

add_executable(
	infographic
	Infographic.cpp
	main.cpp
)

install(
  TARGETS infographic
  RUNTIME DESTINATION bin
)

qt5_use_modules(
	infographic
	Core
)
