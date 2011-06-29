import struct, sys, os

def readUInt32(f):
	return struct.unpack("I", f.read(4))[0]

def readInt32(f):
	return struct.unpack("i", f.read(4))[0]


def readString(f, maxSize):
	s = ""
	pos = 0
	while pos < maxSize:
		c = f.read(1)
		s += c
		pos += 1
		if c == '\0':
			break
		
	return s

def readFloat32(f):
	return struct.unpack("f", f.read(4))[0]

def readVector4(f):
	return (readFloat32(f), readFloat32(f), readFloat32(f), readFloat32(f))

def readVector3(f):
	return (readFloat32(f), readFloat32(f), readFloat32(f))


# dump all parseable info in text format
def readHeader(f):
	# TODO read header
	#char	descriptor[4];			// DVSC
	#uint32	version;				// 100
	#uint32	textureCount;			
	#uint32	materialCount;
	#uint32	lightCount;
	#uint32	cameraCount;
	#uint32	staticMeshCount;
	header = {}
	header['descriptor'] = f.read(4)
	header['version'] = readUInt32(f)
	header['textureCount'] = readUInt32(f)
	header['materialCount'] = readUInt32(f)
	header['lightCount'] = readUInt32(f)
	header['cameraCount'] = readUInt32(f)
	header['staticMeshCount'] = readUInt32(f)
	header['animatedMeshCount'] = readUInt32(f)
	
	return header
	
def readTexture(f):
	texture = {}
	texture['id'] = readUInt32(f)
	texture['name'] = readString(f, 512)
	return texture
	
def readMaterial(f):
	material = {}
	material['id'] = readUInt32(f)
	material['name'] = readString(f, 512)
	
	material['ambient'] = readVector4(f)
	material['diffuse'] = readVector4(f)
	material['diffuseTextureId'] = readUInt32(f)
	material['emission'] = readVector4(f)
	material['indexOfRefraction'] = readFloat32(f)
	material['reflective'] = readVector4(f)
	material['reflectivity'] = readFloat32(f)
	material['shininess'] = readFloat32(f)
	material['specular'] = readVector4(f)
	material['transparency'] = readFloat32(f)
	material['transparent'] = readVector4(f)
	
	return material
	
def readStaticMesh(f):
	polyGroupCount = readInt32(f)
	mesh = {'polygroups':[]}
	for i in range(polyGroupCount):
		polyGroup = {'vertices':[], 'indices':[]}
		vertexCount = readUInt32(f)
		indexCount = readUInt32(f)
		for v in range(vertexCount):
			vert = {}
			vert['position'] = readVector3(f)
			vert['normal'] = readVector3(f)
			vert['texCoords0'] = readVector3(f)
			vert['texCoords1'] = readVector3(f)
			polyGroup['vertices'].append(vert)
		
		for n in range(indexCount):
			ind = readInt32(f)
			polyGroup['indices'].append(ind)
			
		mesh['polygroups'].append(polyGroup)
			
			
	return mesh
	
def readSceneGraph(f):
	pass
	

def dumpFile(fname):
	f = file(fname, "rb")
	header = readHeader(f)
	print("header = %s" % (str(header)))
	
	textures = []
	# textures
	for i in range(header['textureCount']):
		textures.append(readTexture(f))
		
	print("textures = %s" % (str(textures)))
	
	# materials
	materials = []
	for i in range(header['materialCount']):
		materials.append(readMaterial(f))
		
	print("materials = %s" % (str(materials)))
	
	# static meshes
	meshes = []
	for i in range(header['staticMeshCount']):
		meshes.append(readStaticMesh(f))
		
	print("meshes = %s" % (str(meshes)))
	
	# scene graph
	
	f.close()
	
	
	
# --- main ---
if len(sys.argv) > 1:
	dumpFile(sys.argv[1])
else:
	print("specify filename as arg");
	
