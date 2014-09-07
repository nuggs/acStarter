
#define MAX_EVENT_HASH 128

#define EVENT_UNOWNED 0
#define EVENT_OWNER_NONE 1
#define EVENT_OWNER_SYSTEM 2
#define EVENT_OWNER_TRACK 3

/* event types */
#define EVENT_NONE 0

typedef struct event_data EVENT;
typedef bool EVENT_FUN(EVENT *event);

struct event_data {
	EVENT_FUN *fun;
	char *argument;
	int passes;
	int type;
	int owner_type;
	int bucket;
	/*union {
		TRACK *track;
	} owner; */
};

EVENT *alloc_event(void);
void dequeue_event(EVENT *event);
void init_event_queue(int section);
void heartbeat(void);
void add_event_system(EVENT *event, int delay);
