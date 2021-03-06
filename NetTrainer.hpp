#pragma once
#include "NetRunner.hpp"
#include "NN_MCTS.hpp"
#include <thread>

class NetTrainer
{
private:
    std::shared_ptr<NetRunner> runner;
    std::shared_ptr<Game> baseGame;
    std::shared_ptr<NN_Interface> interface;
    std::shared_ptr<Net> net;
    std::shared_ptr<torch::optim::Adam> optimizer;
    std::shared_ptr<torch::Device> device;

    std::vector<std::string> games;
    std::vector<double> gameScores;

    void dataGenLoop();
    void dataGenSupervised();
    void testLoop();
    std::vector<sample> data;
    sample genSample(std::shared_ptr<Game> game);
    std::vector<std::shared_ptr<std::thread>> threads;
    std::mutex data_mutex;
    std::condition_variable data_cv;
    int target_samples;
    int train_batch_size;
    int new_samples = 0;
    void trainEpoch();
    

public:
    NetTrainer(
        std::shared_ptr<Game> game, 
        std::shared_ptr<NN_Interface> interface,
        std::shared_ptr<Net> net,
        std::shared_ptr<torch::Device> device,
        std::shared_ptr<torch::optim::Adam> optimizer,
        int eval_batch_size,
        int train_batch_size
    );
    void train(int target_samples, int train_threads, int test_threads, int supervised_threads);
    void shutdown();
};