
	/* $Id: fpm_events.h,v 1.9 2008/05/24 17:38:47 anight Exp $ */
	/* (c) 2007,2008 Andrei Nigmatulin */

#ifndef FPM_EVENTS_H
#define FPM_EVENTS_H 1

#define FPM_EV_TIMEOUT  (1 << 0) // 
#define FPM_EV_READ     (1 << 1)
#define FPM_EV_PERSIST  (1 << 2)
#define FPM_EV_EDGE     (1 << 3) // 边缘触发事件

#define fpm_event_set_timer(ev, flags, cb, arg) fpm_event_set((ev), -1, (flags), (cb), (arg))

/* 用于存放一个事件 */
struct fpm_event_s {
	int fd;                   /* not set with FPM_EV_TIMEOUT */ // 时间代表的fd，也就是监听的fd
	struct timeval timeout;   /* next time to trigger */ // 下一次触发事件的时间，用于时间事件中
	struct timeval frequency; // 时间事件的触发频率, 毫秒为单位
	void (*callback)(struct fpm_event_s *, short, void *); // 时间触发的回调函数
	void *arg;
	int flags;
	int index;                /* index of the fd in the ufds array */
	short which;              /* type of event */ // 事件类型
};

/* 事件队列节点 */
typedef struct fpm_event_queue_s {
	struct fpm_event_queue_s *prev; // 队列中的上一个事件
	struct fpm_event_queue_s *next; // 队列中的下一个事件
	struct fpm_event_s *ev; // 存放事件内容
} fpm_event_queue;

struct fpm_event_module_s {
	const char *name;
	int support_edge_trigger;
	int (*init)(int max_fd);
	int (*clean)(void);
	int (*wait)(struct fpm_event_queue_s *queue, unsigned long int timeout);
	int (*add)(struct fpm_event_s *ev);
	int (*remove)(struct fpm_event_s *ev);
};

void fpm_event_loop(int err);
void fpm_event_fire(struct fpm_event_s *ev);
int fpm_event_init_main();
int fpm_event_set(struct fpm_event_s *ev, int fd, int flags, void (*callback)(struct fpm_event_s *, short, void *), void *arg);
int fpm_event_add(struct fpm_event_s *ev, unsigned long int timeout);
int fpm_event_del(struct fpm_event_s *ev);
int fpm_event_pre_init(char *machanism);
const char *fpm_event_machanism_name();
int fpm_event_support_edge_trigger();

#endif
