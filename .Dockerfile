FROM g++:4.9
COPY . /usr/src/myapp
WORKDIR /usr/src/myapp
RUN g++ -o myapp program1.cpp
CMD ["./myapp"]
