# integrated modules
# * math
# * fs
# * system
# * thread
# * json
# * net
# * native
# * gl
# * glut
# * sqlite
# * xml
# * gui
# * debug
# * 

# c function call over native module
result = native.ccall("c_function_name_add", 10, 20)

# if and for expression like in python
foo = foo else 20

foo = 10 if x < 10 else 20

foo = 10 if x < 10 else 
	  20 if x < 20 else 30

foo = 10 if x < 10 else 
	  20 if x < 20 else 
	  30 if x < 30 else 40

bar = [x + 1 for x = 0; x < 10; x++]

foo5 = [0:2:100]

bar = range(0, 10)     # generator
foo = range(0, 2, 100) # generator

for i in range(0, 100):
	print "i = " + i
end

for i in [0:100]:
	print "i = " + i
end

foo6 = [x * 2 for x in foo5]

# dictionary to variables
x, y = {
	x = 10,
	y = 20,
}
assert(x == 10)
assert(y == 20)

# dictionary example
dictionaryExample = {
	integerField   = 42
	realField      = 3.1415
	functionField  = fun(self) ret "real field " + self.realField end
	booleanField   = false
	objectField    = Dog("Barky")
	stringField    = "Mac OS"
	nilField       = nil
}

print "dict function field return " + dictionaryExample.functionField()

class TaskQueue
	fun __init__(self)
		self.processList = {
			firstWork = fun()
				print "do hard work"
				sleepSec(3)
			end
		}
		self.isWork = true
		self.th = thread.create(self)
	end

	fun __call__(self)
		isw
		lock(self.isWork) isw = self.isWork end
		while isw:
			proc
			lock(self.processList) 
				while self.processList.len() <= 0 and isw:
					self.processList.wait()
				end
				if self.processList.len() <= 0:
					continue
				end
				proc = self.processList[0] 
				self.processList.remove(0)
			end
			proc()
		end
	end

	fun addWork(self, work)
		lock(self.processList)
			self.processList.append(work)
			self.processList.notifyOne()
		end
		ret self
	end

	fun stop(self)
		lock(self.isWork)
			self.isWork = false
		end
		self.processList.notifyOne()
		if self.th.isJoinable():
			self.th.join()
		end
	end
end

threadBody = TaskQueue().addWork(fun()
	i = 100
	while i > 0:
		print "hui 100 raz"
		i -= 1
	end
end).addWork(fun()
	print "Boo"
end)

thread.create(fun() 
	while true:
	end
end)

# type module type transform
int()
real()
str()
bool()

# debug module
debug.stacktrace()
debug.breakpoint()


file = fs.open("test", fun() end)


# __call__
# __init__
# __copy__
# __dict__
# __add__
# __sub__
# __str__
# __mul__
# __div__
# __adda__
# __suba__
# __mula__
# __diva__
# __mod__
# __moda__
