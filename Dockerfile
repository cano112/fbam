FROM spritsail/alpine-cmake:3.9

ARG enabled
ARG command
ARG logfile
ARG path_filter

ENV HF_VAR_FS_MONIT_ENABLED=$enabled
ENV HF_VAR_FS_MONIT_COMMAND=$command
ENV HF_VAR_FS_MONIT_LOGFILE=$logfile
ENV HF_VAR_FS_MONIT_PATH_FILTER=$path_filter
COPY . ./fbam
RUN ./fbam/docker_test.sh
