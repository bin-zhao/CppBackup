//
// Test.h
//

#ifndef __TEST_20160628_H__
#define __TEST_20160628_H__

namespace TestUtil{

	class Test{
	public:
		static Test* getInstance(){
			return m_instance;
		}

	public:
		Test(){
			if (m_instance){
				delete m_instance;
				m_instance = nullptr;
			}
			m_instance = this;
		}
		virtual ~Test(){}
		virtual int test(int argc, _TCHAR* argv[]){
			return 0;
		}

	protected:
		Test(const Test&){}
		Test& operator=(const Test&){}

	protected:
		static Test* m_instance;
	};

}

#endif // __TEST_20160628_H__