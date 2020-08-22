FROM ubuntu

LABEL description="Container to build and serve neoid"

# install build dependencies
RUN apt-get update && apt-get install -y g++ make

# copy neoid source
COPY src /root/src
COPY Makefile /root/Makefile

# build 
RUN cd /root &&  make

# expose port
expose 18989

ENTRYPOINT ["/root/build/bin/neoid", "-i", "$machine_id", "-b", "1024"]
