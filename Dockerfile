# Use an Ubuntu parent image
FROM ubuntu:17.10

# -qq suppresses output
RUN apt-get -qq update
# Complains about unmet dependencies that are "not going to be installed" without this line
#RUN sudo apt-get -qq -y install libglu1 libglu1-mesa freeglut3 libdevil1c2 libglew1.10 libsdl2-2.0-0 \
#		libglu1-mesa-dev libsdl2-mixer-2.0.0 libsdl2-ttf-2.0-0 libgles2-mesa-dev
# TODO: Figure out how to remove above line
RUN apt-get -qq -y install libbox2d-dev libsdl2-dev libsdl2-ttf-dev libsdl2-mixer-dev \
		libdevil-dev libglew-dev freeglut3-dev libxmu-dev libxi-dev libglm-dev cmake gcc g++


# Based off of http://fabiorehm.com/blog/2014/09/11/running-gui-apps-with-docker/
# Replace 1000 with your user / group id (Find them with "id -u $USER" and "id -g $USER")
RUN export uid=1000 gid=1000 && \
		mkdir -p /home/developer && \
		mkdir -p /etc/sudoers.d && \
		echo "developer:x:${uid}:${gid}:Developer,,,:/home/developer:/bin/bash" >> /etc/passwd && \
		echo "developer:x:${uid}:" >> /etc/group && \
		echo "developer ALL=(ALL) NOPASSWD: ALL" > /etc/sudoers.d/developer && \
		chmod 0440 /etc/sudoers.d/developer && \
		chown ${uid}:${gid} -R /home/developer

# Set the working directory to /planet
WORKDIR /planet

# Copy the current directory contents into the container at /planet
ADD . /planet

WORKDIR /planet/build
RUN cmake ../
RUN make
RUN ln -s ../resources
RUN ln -s ../shaders

USER developer
ENV HOME /home/developer

CMD ./game
