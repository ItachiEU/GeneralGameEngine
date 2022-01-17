#include "NetRunner.hpp"

NetRunner::NetRunner(std::shared_ptr<Net> net, std::shared_ptr<torch::Device> device, int batch_size)
{
   this->net = net;
   this->device = device;
   this->done = false;
   this->runner = std::thread(&NetRunner::run_loop, this);
   this->cnt = 0;
   this->batch_size = batch_size;
   this->output = std::vector<torch::Tensor>(batch_size);
}

torch::Tensor NetRunner::request_run(torch::Tensor input)
{
   std::unique_lock<std::mutex> lck(this->mtx);
   std::condition_variable cv;
   bool ready = false;
   this->to_process.push_back(input);
   int request_pos = this->cnt;
   this->cnt = (this->cnt + 1) % this->batch_size;
   this->to_process_cv.push_back(&cv);
   this->ready_list.push_back(&ready);
   this->runner_cv.notify_one();

   while (!ready)
   {
      cv.wait(lck);
   }
   return this->output[request_pos];
}

void NetRunner::shutdown()
{
   this->done = true;
   this->runner_cv.notify_one();
   this->runner.join();
}

void NetRunner::run_loop()
{
   while (!this->done)
   {
      std::unique_lock<std::mutex> lck(this->mtx);
      this->runner_cv.wait(lck); // wait for a request
      if (this->done)
      {
         break;
      }
      int batch_size = this->to_process.size();
      if (batch_size < this->batch_size)
      {
         continue;
      }
      std::vector<torch::Tensor> inputs(batch_size);
      for (int i = 0; i < batch_size; i++)
      {
         inputs[i] = this->to_process[i];
      }
      torch::Tensor input = torch::cat(inputs);
      torch::Tensor out = this->net->forward(input);
      for (int i = 0; i < batch_size; i++)
      {
         this->output[i] = out.slice(0, i, i + 1);
      }
      for (int i = 0; i < batch_size; i++)
      {
         *(this->ready_list[i]) = true;
         this->to_process_cv[i]->notify_one();
      }
      // Not incredibly efficient, but those vectors should be relatively small
      this->to_process.erase(this->to_process.begin(), this->to_process.begin() + batch_size);
      this->to_process_cv.erase(this->to_process_cv.begin(), this->to_process_cv.begin() + batch_size);
      this->ready_list.erase(this->ready_list.begin(), this->ready_list.begin() + batch_size);
   }
}