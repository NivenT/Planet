# Use an official Python runtime as a parent image
FROM python:2.7-slim

# Set the working directory to /planet
WORKDIR /planet

# Copy the current directory contents into the container at /planet
ADD . /planet /jubilant-funicular

# Define environment variable
ENV NAME PLANET