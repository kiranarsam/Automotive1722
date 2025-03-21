# Use the official Ubuntu 22.04 image from the Docker Hub
FROM ubuntu:22.04
RUN apt-get update && apt-get install -y \
        net-tools \
        can-utils \
        iputils-ping \
        iproute2 \
        && rm -rf /var/lib/apt/lists/*

WORKDIR /app
