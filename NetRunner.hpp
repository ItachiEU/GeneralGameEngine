#pragma once
#include <thread>
#include <mutex>
#include <condition_variable>
#include <torch/torch.h>
#include <vector>

class Net : public torch::nn::Module
{
public:
   virtual torch::Tensor forward(torch::Tensor input) = 0;
   virtual torch::Tensor loss(
      torch::Tensor input, 
      torch::Tensor moves, 
      torch::Tensor masks,
      torch::Tensor move_scores, 
      torch::Tensor result
   ) = 0;
};

class NetRunner
{
private:
   std::thread runner;
   std::vector<torch::Tensor> to_process;
   std::vector<std::condition_variable *> to_process_cv;
   std::vector<bool *> ready_list;
   std::vector<torch::Tensor> output;
   int cnt;
   int batch_size;
   std::mutex mtx;
   std::condition_variable runner_cv;
   bool done;
   std::shared_ptr<Net> net;
   std::shared_ptr<torch::Device> device;
   void run_loop();

public:
   NetRunner(std::shared_ptr<Net> net, std::shared_ptr<torch::Device> device, int batch_size);
   torch::Tensor request_run(torch::Tensor input);
   void shutdown();
};