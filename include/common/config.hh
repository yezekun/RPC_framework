namespace RPC_FRAMEWORK::CONFIG {
class config {
  private:
    /* data */
  public:
    config(/* args */);
    ~config();
    /* socket */
    static constexpr int socket_server_listen_queue_num = 10;
    static constexpr int socket_message_max_size = 64;
};
}; // namespace RPC_FRAMEWORK::CONFIG