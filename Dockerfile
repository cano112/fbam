FROM spritsail/alpine-cmake:3.9
COPY . ./decorator

RUN chmod +x ./decorator/build.sh
RUN ./decorator/build.sh

#RUN LD_PRELOAD=./decorator/build/libblockaccess.so ./decorator/build/testreader ./decorator/testreader/sample_file.txt
#RUN cat /tmp/file_access.log
