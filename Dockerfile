FROM ubuntu:22.04

# Install build tools, CMake, Python, Git, and SQLite
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    python3.10 \
    python3-pip \
    git \
    sqlite3 \
    libsqlite3-dev

# Install Python dependencies
RUN python3.10 -m pip install \
    numpy \
    nibabel

# Clone the MIDSX repository, and all submodules
RUN git clone --recurse-submodules https://github.com/jmeneghini/MIDSX.git /usr/src/MIDSX

# Set the working directory
WORKDIR usr/src/MIDSX

# Build the project
RUN mkdir build && cd build && cmake .. && make




