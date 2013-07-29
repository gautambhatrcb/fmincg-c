import pyopencl as cl
import numpy as np

ctx = cl.create_some_context()
queue = cl.CommandQueue(ctx,properties=cl.command_queue_properties.PROFILING_ENABLE)
mf = cl.mem_flags

# build the fmincg kernel
with open('fmincg.cl','r') as f:
	ker_code = ''.join(f.readlines())
fmicgPrg = cl.Program(ctx, ker_code).build()

# build the kernel that computes cost and gradient
with open('costFunc.cl','r') as f:
	ker_code = ''.join(f.readlines())
costPrg = cl.Program(ctx, ker_code).build()

# prep variables
x = np.zeros((2,1)).astype(np.float32) # initial x vector = (0,0)
d_x = cl.Buffer(ctx, mf.COPY_HOST_PTR, hostbuf=x)

cost = np.zeros(1).astype(np.float32)
d_cost = cl.Buffer(ctx, mf.WRITE_ONLY,cost.nbytes)

grad = np.zeros((2,1)).astype(np.float32)
d_grad = cl.Buffer(ctx, mf.WRITE_ONLY,grad.nbytes)

iState = np.zeros(100).astype(np.int32)
d_iState = cl.Buffer(ctx, mf.COPY_HOST_PTR, hostbuf=iState)

state = np.zeros(100).astype(np.float32)
d_state = cl.Buffer(ctx, mf.COPY_HOST_PTR, hostbuf=state)

# call the cost function and fmincg kernel repititively
for i in range(1000):
	costPrg.costFunc(queue,(1,),(1,),d_x,d_cost,d_grad)
	fmicgPrg.fmincg(queue,(1,),(1,),d_x,d_cost,d_grad,d_iState,d_state)

cl.enqueue_copy(queue, x, d_x).wait()
print "X-optimum = \n",x
