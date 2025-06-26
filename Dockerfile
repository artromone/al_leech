FROM ubuntu:22.04

# Установка зависимостей
RUN apt-get update && apt-get install -y \
  build-essential \
  cmake \
  make \
  libsfml-dev \
  libgl1-mesa-dev \
  libglu1-mesa-dev \
  libegl1-mesa \
  libx11-dev \
  libxrandr-dev \
  libxinerama-dev \
  libxcursor-dev \
  libxi-dev \
  && rm -rf /var/lib/apt/lists/*

# Создание рабочей директории
WORKDIR /app

# Копирование исходного кода
COPY . .

# Сборка проекта
RUN make local

# Команда по умолчанию
CMD ["./sfml-app"]
