# musical deformation of the objects: kick = breathing, snare = radial shock wave,
# melody (pitches) = twist, bass/mids = undulations, hi-hat = sparkle
m0 = sqrt(x*x+y*y+z*z)
m1 = 1+0.035*kick+0.015*l0+0.04*snare*sin(m0*4.5-(1-snare)*10)
m2 = ((p1+p2)*0.5-0.4)*1.0*y+spin*0.35
m3 = x*cos(m2)-z*sin(m2)
z = x*sin(m2)+z*cos(m2)
x = m3+sin(y*2.7+t*1.9)*(l1+0.6)*0.07
z = z+sin(x*2.3-t*1.3)*(l2+0.6)*0.07
y = y+sin(z*3.1-t*2.3)*(h3+h1+0.4)*0.06
m4 = hat*0.012
x = (x+sin(idx*0.37+t*40)*m4)*m1
y = (y+sin(idx*0.61+t*53)*m4)*m1
z = (z+sin(idx*0.83+t*47)*m4)*m1
