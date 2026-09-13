# mingw32-make -j $(nproc)

# 1. Настройки компилятора и флагов
ASLOV    := ../../aslov/aslov/src/aslov
ESTIMATOR:= ../../parser/parser/cpp/estimator/src/estimator
CXX      := g++
#-O2 -Wall -Wextra -Wpedantic
CXXFLAGS := -O3 -DNDEBUG -Wall -std=c++23 -I$(ASLOV) -I$(ESTIMATOR) $(shell pkg-config --cflags gtk+-3.0)
LIBS     := $(shell pkg-config --libs gtk+-3.0)

TARGET   := graph

ifeq ($(OS),Windows_NT)
    LDFLAGS += -mwindows
endif

# 2. Автоматический поиск файлов
# Находит все файлы .cpp в текущей директории
SRCS     := $(wildcard src/*.cpp) $(wildcard $(ASLOV)/aslov.cpp) $(wildcard $(ESTIMATOR)/*.cpp)
# Меняет расширение .cpp на .o для создания списка объектных файлов
OBJS     := $(SRCS:.cpp=.o)

EXTRA_OBJS := $(TARGET)/icon/app.o

# 3. Главный таргет (выполняется по команде `make`)
all: $(TARGET)

# Сборка исполняемого файла из объектных
$(TARGET): $(OBJS) $(EXTRA_OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(EXTRA_OBJS) $(LDFLAGS) $(LIBS)

# Правило для компиляции каждого файла .cpp в .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 4. Вспомогательные команды
# Очистка проекта от временных файлов (запуск: `make clean`)
clean:
	rm -f $(OBJS) $(TARGET).exe

# Указываем, что эти цели не являются реальными файлами
.PHONY: all clean
