# Nombre del ejecutable
TARGET = programa

# Compilador
CXX = g++
# Opciones de compilación
CXXFLAGS = -std=c++17 -Wall

# Carpeta de compilación
OBJDIR = build

# Archivos fuente
SRCS = main.cpp disk.cpp console.cpp storageManager.cpp diskController.cpp

# Archivos objeto en carpeta build/
OBJS = $(SRCS:.cpp=.o)
OBJS := $(addprefix $(OBJDIR)/, $(OBJS))

# Regla principal
all: $(TARGET)

# Cómo construir el ejecutable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Crear carpeta build y compilar cada .cpp
$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Limpieza de archivos intermedios
clean:
	rm -rf $(OBJDIR) $(TARGET)