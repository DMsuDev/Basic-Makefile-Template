FROM ubuntu:26.04

RUN apt update && apt install -y build-essential clang

COPY . /app

WORKDIR /app