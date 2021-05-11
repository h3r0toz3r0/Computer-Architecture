FROM centos:7 AS build
RUN yum -y update
RUN yum install -y \
        wget && yum clean all

WORKDIR /root
RUN wget http://www.iozone.org/src/current/iozone-3-491.x86_64.rpm
RUN yum install -y iozone-3-491.x86_64.rpm
ARG SIZE
ARG RECLEN
CMD ["/opt/iozone/bin/iozone", "-RaI", "-r ${RECLEN}", "-s ${SIZE}", "-i 0", "-i 1"]