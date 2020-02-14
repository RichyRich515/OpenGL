#version 420

uniform mat4 matModel;					// Model or World
uniform mat4 matModelInverseTranspose;	// inverse transpose
uniform mat4 matView;					// View or Camera
uniform mat4 matProjection;				// Projection

uniform float offset;

in vec4 vColour;
in vec4 vPosition;
in vec4 vNormal;
in vec4 vUVx2;
in vec4 vTangent;				// For bump mapping
in vec4 vBiNormal;				// For bump mapping
in vec4 vBoneID;				// For skinned mesh (FBX)
in vec4 vBoneWeight;			// For skinned mesh (FBX)

out vec4 fColour;
out vec4 fVertWorldLocation;
out vec4 fNormal;
out vec4 fUVx2;

// x: offsetX
// y: offsetY
// z: scale
// w: tiling
uniform vec4 heightparams;
uniform sampler2D heightSamp;

uniform vec2 waterOffset;

const int MAXNUMBEROFBONES = 100;
uniform mat4 matBonesArray[MAXNUMBEROFBONES];
uniform bool isSkinnedMesh;

void main()
{
	vec4 vertPosition = vPosition;
	vertPosition += vNormal * offset;

	
	// Height map
	if (heightparams.w != 0.0)
	{
		vec4 samp = texture(heightSamp, vUVx2.st * heightparams.w + waterOffset);
		vertPosition.y += ((samp.r - 1 + samp.g - 1) + samp.b) * heightparams.z;
	}

	if (isSkinnedMesh)
	{
		// *************************************************
		mat4 BoneTransform = matBonesArray[ int(vBoneID[0]) ] * vBoneWeight[0];
		     BoneTransform += matBonesArray[ int(vBoneID[1]) ] * vBoneWeight[1];
		     BoneTransform += matBonesArray[ int(vBoneID[2]) ] * vBoneWeight[2];
		     BoneTransform += matBonesArray[ int(vBoneID[3]) ] * vBoneWeight[3];

		// Apply the bone transform to the vertex:
		vec4 vertOriginal = vec4(vertPosition.xyz, 1.0f);

		vec4 vertAfterBoneTransform = BoneTransform * vertOriginal;
		
		mat4 matMVP = matProjection * matView * matModel;
		// Transform the updated vertex location (from the bone)
		//  and transform with model view projection matrix (as usual)
		gl_Position = matMVP * vertAfterBoneTransform;
		
		
		// Then we do the normals, etc.
		fVertWorldLocation = matModel * vertAfterBoneTransform;	
		
		// Updated "world" or "model" transform 
		mat4 matModelAndBone = matModel * BoneTransform;
		
		vec3 theNormal = normalize(vNormal.xyz);
		fNormal = inverse(transpose(matModelAndBone)) * vec4(theNormal, 1.0);
		fNormal.xyz = normalize(fNormal.xyz); 
		
		fColour = vColour;	
		fUVx2 = vUVx2;

		
		// *************************************************
	}
	else
	{		
		mat4 matMVP = matProjection * matView * matModel;
		gl_Position = matMVP * vec4(vertPosition.xyz, 1.0);
		// Calculate vertex in world space
		fVertWorldLocation = matModel * vec4(vertPosition.xyz, 1.0);
	
		vec3 theNormal = normalize(vNormal.xyz);
		fNormal = matModelInverseTranspose * vec4(theNormal, 1.0);
		fNormal.xyz = normalize(fNormal.xyz); 

		fColour = vColour;
		fUVx2 = vUVx2;
		
	}//if (isSkinnedMesh)

  
}