# Переменные
IMAGE_NAME = sfml-app
CONTAINER_NAME = sfml-container
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
LIBS = -lsfml-graphics -lsfml-window -lsfml-system

# Исходные файлы
SRCDIR = src
SOURCES = $(SRCDIR)/main.cpp \
          $(SRCDIR)/game/Game.cpp \
          $(SRCDIR)/terrain/TerrainManager.cpp \
          $(SRCDIR)/entities/Worm.cpp \
          $(SRCDIR)/entities/Projectile.cpp

TARGET = sfml-app

# Локальная сборка
local: $(SOURCES)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES) $(LIBS)

# Сборка Docker образа
build:
	docker build -t $(IMAGE_NAME) .

# Запуск с поддержкой GUI (без монтирования исходников)
run:
	xhost +local:docker
	docker run --rm \
		--name $(CONTAINER_NAME) \
		-e DISPLAY=$(DISPLAY) \
		-v /tmp/.X11-unix:/tmp/.X11-unix:rw \
		--device /dev/dri \
		$(IMAGE_NAME)

# Запуск для разработки (с монтированием и пересборкой)
dev:
	xhost +local:docker
	docker run --rm \
		--name $(CONTAINER_NAME) \
		-e DISPLAY=$(DISPLAY) \
		-v /tmp/.X11-unix:/tmp/.X11-unix:rw \
		-v $(PWD):/app \
		--device /dev/dri \
		-w /app \
		$(IMAGE_NAME) \
		bash -c "make local && ./$(TARGET)"

# Очистка
clean:
	rm -f $(TARGET)
	docker rmi $(IMAGE_NAME) || true
	xhost -local:docker

.PHONY: build run dev clean local
