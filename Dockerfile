FROM gcc:latest
WORKDIR /app
COPY main.cpp .
COPY httplib.h .
RUN g++ -std=c++17 main.cpp -o server -lpthread -static-libstdc++ -static-libgcc
EXPOSE 8080
CMD ["./server"]