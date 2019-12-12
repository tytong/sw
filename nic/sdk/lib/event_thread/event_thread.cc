//------------------------------------------------------------------------------
// {C} Copyright 2019 Pensando Systems Inc. All rights reserved
//------------------------------------------------------------------------------
#include <memory>
#include <mutex>
#include <set>

#include "event_thread.hpp"

#include "include/sdk/mem.hpp"
#include "lib/ipc/subscribers.hpp"

#define MAX_THREAD_ID 63

namespace sdk {
namespace event_thread {

typedef enum updown_status_ {
    THREAD_DOWN = 0,
    THREAD_UP,
} updown_status_t;

typedef struct updown_meta_ {
    updown_status_t status;
    uint32_t thread_id;
} updown_meta_t;

typedef union lfq_msg_meta_ {
    updown_meta_t updown;
} lfq_msg_meta_t;

class lfq_msg {
public:
    static lfq_msg *factory(void);
    static void destroy(lfq_msg *msg);
    enum lfq_msg_type {
        USER_MSG,
        UPDOWN_MSG, // thread-up notifications
    } type;
    void *payload;
    lfq_msg_meta_t meta;
private:
    lfq_msg();
    ~lfq_msg();
};
typedef std::shared_ptr<lfq_msg> lfq_msg_ptr;

// Responsible for thread status notification
// Thread going up and down
class updown_mgr {
public:
    void subscribe(uint32_t subscriber, uint32_t target);
    void up(uint32_t thread_id);
private:
    std::map<uint32_t, updown_status_t> status_;
    std::map<uint32_t, std::set<uint32_t> > subscriptions_;
    std::mutex mutex_;
};
typedef std::shared_ptr<updown_mgr> updown_mgr_ptr;

static thread_local event_thread *t_event_thread_ = NULL;

static event_thread *g_event_thread_table[MAX_THREAD_ID + 1];

static updown_mgr g_updown;

typedef struct ipc_watcher_ {
    sdk::ipc::handler_cb callback;
    const void *ctx;
} ipc_watcher_t;

// Converts ev values to event values.
// e.g. EV_READ to EVENT_READ
static int
ev_to_event (int ev_value) {
    int event_value = 0;

    if (EV_READ & ev_value) {
        event_value |= EVENT_READ;
    }
    if (EV_WRITE & ev_value) {
        event_value |= EVENT_WRITE;
    }

    return event_value;
}

// Converts event values to ev values
// e.g. EVENT_READ to EV_READ
static int
event_to_ev (int event_value) {
    int ev_value = 0;

    if (EVENT_READ & event_value) {
        ev_value |= EV_READ;
    }
    if (EVENT_WRITE & event_value) {
        ev_value |= EV_WRITE;
    }

    return ev_value;
}

lfq_msg *
lfq_msg::factory(void) {
    void    *mem;
    lfq_msg *new_msg;

    mem = SDK_CALLOC(SDK_MEM_ALLOC_LFQ_MSG, sizeof(*new_msg));
    if (!mem) {
        return NULL;
    }
    new_msg = new (mem) lfq_msg();

    return new_msg;
}

void
lfq_msg::destroy(lfq_msg *msg) {
    assert(msg != NULL);
    msg->~lfq_msg();
    SDK_FREE(SDK_MEM_ALLOC_LFQ_MSG, msg);
}

lfq_msg::lfq_msg() {
}

lfq_msg::~lfq_msg() {
}

void
updown_mgr::subscribe(uint32_t subscriber, uint32_t target) {
    assert(subscriber != target);
    assert(subscriber <= MAX_THREAD_ID);
    assert(target <= MAX_THREAD_ID);
    this->mutex_.lock();
    if (this->status_[target] == THREAD_UP) {
        // send notification
        assert(g_event_thread_table[subscriber] != NULL);
        g_event_thread_table[subscriber]->handle_thread_up(target);
    }
    this->subscriptions_[target].insert(subscriber);
    this->mutex_.unlock();
}

void
updown_mgr::up(uint32_t thread_id) {
    assert(thread_id <= MAX_THREAD_ID);
    this->mutex_.lock();
    assert(this->status_[thread_id] != THREAD_UP);
    this->status_[thread_id] = THREAD_UP;
    for (auto subscriber: this->subscriptions_[thread_id]) {
        // send notification
        assert(g_event_thread_table[subscriber] != NULL);
        g_event_thread_table[subscriber]->handle_thread_up(thread_id);
    }
    this->mutex_.unlock();
}

event_thread *
event_thread::factory(const char *name, uint32_t thread_id,
                      sdk::lib::thread_role_t thread_role, uint64_t cores_mask,
                      loop_init_func_t init_func, loop_exit_func_t exit_func,
                      message_cb message_cb, uint32_t prio,
                      int sched_policy, bool can_yield) {
    int          rv;
    void         *mem;
    event_thread *new_thread;

    mem = SDK_CALLOC(SDK_MEM_ALLOC_LIB_EVENT_THREAD, sizeof(event_thread));
    if (!mem) {
        return NULL;
    }

    new_thread = new (mem) event_thread();
    rv = new_thread->init(name, thread_id, thread_role, cores_mask,
                          init_func, exit_func, message_cb, prio,
                          sched_policy, can_yield);
    if (rv < 0) {
        new_thread->~event_thread();
        SDK_FREE(SDK_MEM_ALLOC_LIB_EVENT_THREAD, new_thread);
        return NULL;
    }

    return new_thread;
}

void
event_thread::destroy(event_thread *thread)
{
    thread->~event_thread();
    SDK_FREE(SDK_MEM_ALLOC_LIB_EVENT_THREAD, thread);
}

int
event_thread::init(const char *name, uint32_t thread_id,
                   sdk::lib::thread_role_t thread_role, uint64_t cores_mask,
                   loop_init_func_t init_func, loop_exit_func_t exit_func,
                   message_cb message_cb, uint32_t prio, int sched_policy,
                   bool can_yield) {
    int rc;

    if (thread_id > MAX_THREAD_ID) {
        return -1;
    }
    assert(g_event_thread_table[thread_id] == NULL);

    rc = thread::init(name, thread_id, thread_role, cores_mask,
                      &event_thread_entry_, prio, sched_policy, can_yield);
    if (rc < 0) {
        return rc;
    }

    this->loop_ = ev_loop_new(0);
    if (this->loop_ == NULL) {
        return -1;
    }

    this->init_func_ = init_func;
    this->exit_func_ = exit_func;
    this->message_cb_ = message_cb;
    this->user_ctx_ = NULL;

    // The async watcher is for getting messages from different threads
    this->async_watcher_.data = this;
    ev_async_init(&this->async_watcher_, event_thread::async_callback_);
    ev_async_start(this->loop_, &this->async_watcher_);

    g_event_thread_table[thread_id] = this;

    return 0;
}

event_thread::event_thread() {
}

event_thread::~event_thread() {
    g_event_thread_table[this->thread_id()] = NULL;
}

void*
event_thread::event_thread_entry_(void *ctx) {
    ((event_thread *)ctx)->run_();
    return NULL;
}

void
event_thread::async_callback_(struct ev_loop *loop, ev_async *watcher,
                              int revents) {
    // our class is watcher->data
    ((event_thread *)(watcher->data))->handle_async_();
}

void
event_thread::handle_async_(void) {
    if (this->stop_) {
        ev_break(this->loop_, EVBREAK_ONE);
        return;
    }

    this->process_lfq_();
}

void
event_thread::handle_thread_up(uint32_t thread_id) {
    lfq_msg *msg = lfq_msg::factory();
    msg->type = lfq_msg::UPDOWN_MSG;
    msg->meta.updown.thread_id = thread_id;
    msg->meta.updown.status = THREAD_UP;
    this->message_send(msg);
}

void
event_thread::process_lfq_(void) {
    lfq_msg *msg;
    while (true) {
        msg = (lfq_msg *)this->dequeue();
        if (msg == NULL) {
            return;
        }
        if (msg->type == lfq_msg::USER_MSG) {
            assert(this->message_cb_ != NULL);
            this->message_cb_(msg->payload, this->user_ctx_);
        } else if (msg->type == lfq_msg::UPDOWN_MSG) {
            assert(this->updown_up_cbs_.count(msg->meta.updown.thread_id) > 0);
            this->updown_up_cbs_[msg->meta.updown.thread_id](
                msg->meta.updown.thread_id,
                this->updown_up_ctxs_[msg->meta.updown.thread_id]);
        } else {
            assert(false);
        }
        lfq_msg::destroy(msg);
    }
}

static void
ipc_io_callback (struct ev_loop *loop, ev_io *watcher, int revents)
{
    ipc_watcher_t *ipc_watcher = (ipc_watcher_t *)watcher->data;

    ipc_watcher->callback(watcher->fd, ipc_watcher->ctx);
}

void
event_thread::create_ipc_watcher_(int fd, sdk::ipc::handler_cb cb,
                                  const void *ctx) {
    // todo: fix me: we are leaking!
    ipc_watcher_t *ipc_watcher = (ipc_watcher_t *)malloc(sizeof(*ipc_watcher));
    ipc_watcher->callback = cb;
    ipc_watcher->ctx = ctx;

    // todo: fix me: we are leaking!
    ev_io *watcher = (ev_io *)malloc(sizeof(*watcher));
    watcher->data = ipc_watcher;

    ev_io_init(watcher, ipc_io_callback, fd, EV_READ);
    ev_io_start(this->loop_, watcher);
}

void
event_thread::create_ipc_watcher (int fd, sdk::ipc::handler_cb cb,
                                  const void *ctx,
                                  const void *ipc_poll_fd_ctx)
{
    event_thread *thread = (event_thread *)ipc_poll_fd_ctx;

    thread->create_ipc_watcher_(fd, cb, ctx);
}

void
event_thread::run_(void) {
    t_event_thread_ = this;

    sdk::ipc::ipc_init_async(this->thread_id(), create_ipc_watcher, this);
    
    if (this->init_func_) {
        this->init_func_(this->user_ctx_);
    }
    this->set_ready(true);

    g_updown.up(this->thread_id());

    ev_run(this->loop_, 0);

    if (this->exit_func_) {
        this->exit_func_(this->user_ctx_);
    }

    ev_loop_destroy(this->loop_);
    this->loop_ = NULL;
}

sdk_ret_t
event_thread::start(void *ctx) {
    // This function will be called from different thread
    // No locking required
    this->user_ctx_ = ctx;
    thread::start(this);

    return SDK_RET_OK;
}

sdk_ret_t
event_thread::stop(void) {
    // This function can be called from different thread
    // No locking required
    this->stop_ = true;
    this->set_running(false);

    ev_async_send(this->loop_, &this->async_watcher_);

    return SDK_RET_OK;
}

void
event_thread::updown_up_subscribe(uint32_t thread_id, updown_up_cb callback,
                                  void *ctx)
{
    assert(t_event_thread_ == this);
    assert(thread_id <= MAX_THREAD_ID);
    assert(callback != NULL);
    assert(this->updown_up_cbs_.count(thread_id) == 0);

    this->updown_up_cbs_[thread_id] = callback;
    this->updown_up_ctxs_[thread_id] = ctx;

    g_updown.subscribe(this->thread_id(), thread_id);
}

static void
ev_prepare_callback_ (struct ev_loop *loop, ev_prepare *watcher, int revents)
{
    prepare_t *prepare = (prepare_t *)watcher;
    prepare->callback(prepare, prepare->ctx);
}

void
event_thread::prepare_start(prepare_t *prepare)
{
    assert(t_event_thread_ == this);
    assert(prepare->ev_watcher.cb == ev_prepare_callback_);

    ev_prepare_start(this->loop_, &prepare->ev_watcher);
}

void
event_thread::prepare_stop(prepare_t *prepare)
{
    assert(t_event_thread_ == this);

    ev_prepare_stop(this->loop_, &prepare->ev_watcher);
}

static void
ev_io_callback_ (struct ev_loop *loop, ev_io *watcher, int revents)
{
    io_t *io = (io_t *)watcher;
    io->callback(io, watcher->fd, ev_to_event(revents));
}

void
event_thread::io_start(io_t *io) {
    assert(t_event_thread_ == this);
    assert(io->ev_watcher.cb == ev_io_callback_);

    ev_io_start(this->loop_, &io->ev_watcher);
}

void
event_thread::io_stop(io_t *io) {
    assert(t_event_thread_ == this);

    ev_io_stop(this->loop_, &io->ev_watcher);
}

static void
ev_timer_callback_ (struct ev_loop *loop, ev_timer *watcher, int revents)
{
    timer_t *timer = (timer_t *)watcher;
    timer->callback(timer);
}

void
event_thread::timer_start(timer_t *timer) {
    assert(t_event_thread_ == this);
    assert(timer->ev_watcher.cb == ev_timer_callback_);

    ev_timer_start(this->loop_, &timer->ev_watcher);
}

void
event_thread::timer_stop(timer_t *timer) {
    assert(t_event_thread_ == this);

    ev_timer_stop(this->loop_, &timer->ev_watcher);
}

void
event_thread::timer_again(timer_t *timer) {
    assert(t_event_thread_ == this);

    ev_timer_again(this->loop_, &timer->ev_watcher);
}

void
event_thread::message_send(void *message) {
    // This function can be called from different thread
    // No locking required

    this->enqueue(message);
    ev_async_send(this->loop_, &this->async_watcher_);
}

void
updown_up_subscribe (uint32_t thread_id, updown_up_cb cb, void *ctx)
{
    assert(t_event_thread_ != NULL);

    t_event_thread_->updown_up_subscribe(thread_id, cb, ctx);
}

void
prepare_init (prepare_t *prepare, prepare_cb cb, void *ctx)
{
    prepare->callback = cb;
    prepare->ctx = ctx;
    ev_prepare_init(&prepare->ev_watcher, &ev_prepare_callback_);
}

void
prepare_start (prepare_t *prepare)
{
    assert(t_event_thread_ != NULL);

    t_event_thread_->prepare_start(prepare);
}

void
prepare_stop (prepare_t *prepare)
{
    assert(t_event_thread_ != NULL);

    t_event_thread_->prepare_stop(prepare);
}

void
io_init (io_t *io, io_cb callback, int fd, int events)
{
    io->callback = callback;
    ev_io_init(&io->ev_watcher, &ev_io_callback_, fd,
               event_to_ev(events));
}

void
io_start (io_t *io)
{
    // We can only add and remove events from inside the context of the thread
    assert(t_event_thread_ != NULL);

    t_event_thread_->io_start(io);
}

void
io_stop (io_t *io)
{
    // We can only add and remove events from inside the context of the thread
    assert(t_event_thread_ != NULL);

    t_event_thread_->io_stop(io);
}


void
timer_init (timer_t *timer, timer_cb callback,
            double initial_delay, double repeat)
{
    timer->callback = callback;
    ev_timer_init(&timer->ev_watcher, &ev_timer_callback_, initial_delay,
        repeat);
}

void
timer_start (timer_t *timer)
{
    // We can only add and remove events from inside the context of the thread
    assert(t_event_thread_ != NULL);

    t_event_thread_->timer_start(timer);
}

void
timer_stop (timer_t *timer)
{
    // We can only add and remove events from inside the context of the thread
    assert(t_event_thread_ != NULL);

    t_event_thread_->timer_stop(timer);
}

void
timer_again (timer_t *timer)
{
    // We can only manipulate events from inside the context of the thread
    assert(t_event_thread_ != NULL);

    t_event_thread_->timer_again(timer);
}

void
message_send (uint32_t thread_id, void *message)
{
    assert(thread_id <= MAX_THREAD_ID);
    assert(g_event_thread_table[thread_id] != NULL);

    lfq_msg *msg = lfq_msg::factory();
    msg->type = lfq_msg::USER_MSG;
    msg->payload = message;
    g_event_thread_table[thread_id]->message_send(msg);
}

} // namespace event_thread
} // namespace sdk
