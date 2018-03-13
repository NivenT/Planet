# Use an official Python runtime as a parent image
FROM python:2.7-slim

# Set the working directory to /planet
WORKDIR /planet

# Copy the current directory contents into the container at /planet
ADD . /planet

# -qq suppresses output
RUN apt-get -qq update
RUN apt-get -qq -y install libbox2d-dev libsdl2-dev libsdl2-ttf-dev libsdl2-mixer-dev libdevil-dev libglew-dev cmake gcc g++
RUN mkdir build

WORKDIR /planet/build
RUN cmake ../
RUN ln -s ../resources
RUN ln -s ../shaders

CMD ["./game"]