#ifndef __calyx_event_module_h__
#define __calyx_event_module_h__

#include <queue>

//namespace love
//{
//    namespace event
//    {
//        class Message : public Object
//        {
//        private:
//            std::string name;
//            Variant *args[4];
//            int nargs;
//
//        public:
//            Message(const std::string &name, Variant *a = NULL, Variant *b = NULL, Variant *c = NULL, Variant *d = NULL);
//            ~Message();
//
//            int toLua(lua_State *L);
//            static Message *fromLua(lua_State *L, int n);
//        };
//
//        class Event : public Module
//        {
//        public:
//            Event();
//            virtual ~Event();
//
//            void push(Message *msg);
//            bool poll(Message *&msg);
//            virtual void clear();
//
//            virtual void pump() = 0;
//
//        protected:
//            thread::Mutex *mutex;
//            std::queue<Message *> queue;
//
//        }; // Event
//
//    } // event
//} // love

namespace calyx {
    namespace event {

        class EventQueue
        {
        public:
            virtual void PostEvent(int event);
            int PumpEvent();
            void DispatchEvent(int event);

        private:
            std::queue<int>	m_queue;	// 消息队列
        };

    }   // event
}  // calyx

#endif // __calyx_event_module_h__