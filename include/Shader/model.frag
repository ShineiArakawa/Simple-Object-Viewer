#version 330

in vec2 f_uv;
in vec3 f_worldPos;
in vec3 f_color;
in vec3 f_normal;
in vec3 f_barycentric;
in float f_id;
in vec3 f_positionCameraSpace;
in vec3 f_normalCameraSpace;
in vec3 f_lightPosCameraSpace;
in vec4 f_positionLightScreenSpace;

uniform float u_renderType;
uniform float u_bumpMap;
uniform float u_wireFrameMode;
uniform vec3 u_wireFrameColor;
uniform float u_wireFrameWidth;
uniform float u_ambientIntensity;
uniform vec3 u_ambientColor;
uniform vec3 u_diffuseColor;
uniform vec3 u_specularColor;
uniform float u_shininess;
uniform vec3 u_lightPos;

uniform sampler2D u_ambientTexture;
uniform sampler2D u_diffuseTexture;
uniform sampler2D u_specularTexture;
uniform sampler2D u_normalMap;
uniform sampler2D u_depthTexture;

uniform float u_hasAmbientTexture;
uniform float u_hasDiffuseTexture;
uniform float u_hasSpecularTexture;

out vec4 out_color;

vec3 getAmbientColor() {
    if(u_hasAmbientTexture > 0.5) {
        return texture(u_ambientTexture, f_uv).xyz;
    }
    return u_ambientColor;
}

vec3 getDiffuseColor() {
    if(u_hasDiffuseTexture > 0.5) {
        return texture(u_diffuseTexture, f_uv).xyz;
    }
    return u_diffuseColor;
}

vec3 getSpecularColor() {
    if(u_hasSpecularTexture > 0.5) {
        return texture(u_specularTexture, f_uv).xyz;
    }
    return u_specularColor;
}

vec3 calcLocalNormal() {
    vec3 dPdx = dFdx(f_worldPos);
    vec3 dPdy = dFdy(f_worldPos);

    vec2 dUdx = dFdx(f_uv); /* (du/dx, dv/dx) */
    vec2 dUdy = dFdy(f_uv); /* (du/dy, dv/dy) */

    float det = abs(dUdx[0] * dUdy[1] - dUdx[1] * dUdy[0]);

    float invMat00 = dUdy[1] / det;
    float invMat01 = -dUdx[1] / det;
    float invMat10 = -dUdy[0] / det;
    float invMat11 = dUdx[0] / det;

    vec3 localX = normalize(invMat00 * dPdx + invMat01 * dPdy);
    vec3 localY = normalize(invMat10 * dPdx + invMat11 * dPdy);
    vec3 localZ = cross(localX, localY);

    vec4 normalFromMap = texture(u_normalMap, f_uv) * 2.0 - 1.0;

    return normalize(normalFromMap.x * localX + normalFromMap.y * localY + normalFromMap.z * localZ);
}

float calcShadow(vec4 fragPosLightSpace) {
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(u_depthTexture, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float shadow = currentDepth > closestDepth ? 1.0 : 0.0;

    return shadow;
}

vec4 shading(
    vec3 diffuseColor,
    vec3 specularColor,
    vec3 ambientColor,
    vec3 N
) {
    vec3 V = normalize(-f_positionCameraSpace);
    vec3 L = normalize(f_lightPosCameraSpace - f_positionCameraSpace);
    vec3 H = normalize(V + L);

    float ndotl = max(0.0, dot(N, L));
    float ndoth = max(0.0, dot(N, H));

    vec3 diffuse = diffuseColor * ndotl;
    vec3 specular = specularColor * pow(ndoth, u_shininess);
    vec3 ambient = u_ambientIntensity * ambientColor;

    float shadow = calcShadow(f_positionLightScreenSpace);

    return vec4((1.0 - shadow) * (diffuse + specular) + ambient, 1.0);
}

void main() {
    // ================================================================================================================================
    // Wire frame
    // ================================================================================================================================
    if(u_wireFrameMode > 0.5 && u_wireFrameMode < 1.5) {
        // Wire frame
        if(f_barycentric.x < u_wireFrameWidth || f_barycentric.y < u_wireFrameWidth || f_barycentric.z < u_wireFrameWidth) {
            out_color = vec4(u_wireFrameColor, 1.0);
            return;
        }
    } else if(u_wireFrameMode > -1.5 && u_wireFrameMode < -0.5) {
        // Wire frame only
        if(f_barycentric.x < u_wireFrameWidth || f_barycentric.y < u_wireFrameWidth || f_barycentric.z < u_wireFrameWidth) {
            out_color = vec4(u_wireFrameColor, 1.0);
        } else {
            out_color = vec4(u_wireFrameColor, 0.0);
        }
        return;
    }

    // ================================================================================================================================
    // Normal
    // ================================================================================================================================
    vec3 N;
    if(u_bumpMap > 0.5) {
        N = calcLocalNormal();
    } else {
        N = normalize(f_normalCameraSpace);
    }

    // ================================================================================================================================
    // Render
    // ================================================================================================================================
    if(u_renderType > 3.5 && u_renderType < 4.5) {
        // Material shading
        out_color = shading(u_diffuseColor, u_specularColor, u_ambientColor, N);
    } else if(u_renderType > 2.5 && u_renderType < 3.5) {
        // Texture with shading
        out_color = shading(getDiffuseColor(), getSpecularColor(), getAmbientColor(), N);
    } else if(u_renderType > 1.5 && u_renderType < 2.5) {
        // Color with shading
        out_color = shading(f_color, vec3(1.0), f_color, N);
    } else if(u_renderType > 0.5 && u_renderType < 1.5) {
        // Texture
        out_color = texture(u_diffuseTexture, f_uv);
    } else if(u_renderType > -0.5 && u_renderType < 0.5) {
        // Color
        out_color = vec4(f_color, 1.0);
    } else if(u_renderType > -1.5 && u_renderType < -0.5) {
        // Face Normal
        vec3 dfdx = dFdx(f_worldPos);
        vec3 dfdy = dFdy(f_worldPos);
        vec3 color = normalize(cross(dfdx, dfdy));
        color = (color + 1.0) / 2.0;
        out_color = vec4(color, 1.0);
    } else if(u_renderType > -2.5 && u_renderType < -1.5) {
        // Mask
        out_color = vec4(1.0, 1.0, 1.0, 1.0);
    } else if(u_renderType > -3.5 && u_renderType < -2.5) {
        // Vertex normal
        out_color = vec4(f_normal, 1.0);
    }
}