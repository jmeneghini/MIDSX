FROM ubuntu:22.04

# Install build tools, CMake, Python, Git, and SQLite
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    python3.10 \
    python3-pip \
    git \
    sqlite3 \
    libsqlite3-dev \
    libboost-all-dev

# Install Python dependencies
RUN python3.10 -m pip install \
    numpy \
    nibabel

# Make a directory for MIDSX
RUN mkdir /usr/src/MIDSX

# Set the working directory
WORKDIR /usr/src/MIDSX

# Copy the source code into the container
COPY . .

# Build and install MIDSX
RUN mkdir build && cd build && cmake .. && make install

# Build the cpp_sims
WORKDIR /usr/src/MIDSX
RUN ./utility_scripts/build_cpp_sims.sh




