#version 130
uniform sampler2D jagged_textures;
varying highp vec2 textureCoordinate;
varying highp vec2 leftTextureCoordinate;
varying highp vec2 rightTextureCoordinate;

varying highp vec2 topTextureCoordinate;
varying highp vec2 topLeftTextureCoordinate;
varying highp vec2 topRightTextureCoordinate;

varying highp vec2 bottomTextureCoordinate;
varying highp vec2 bottomLeftTextureCoordinate;
varying highp vec2 bottomRightTextureCoordinate;

void main()
{
    lowp float bottomColor = texture2D(jagged_textures, bottomTextureCoordinate).r;
    lowp float bottomLeftColor = texture2D(jagged_textures, bottomLeftTextureCoordinate).r;
    lowp float bottomRightColor = texture2D(jagged_textures, bottomRightTextureCoordinate).r;
    lowp vec4 centerColor = texture2D(jagged_textures, textureCoordinate);
    lowp float leftColor = texture2D(jagged_textures, leftTextureCoordinate).r;
    lowp float rightColor = texture2D(jagged_textures, rightTextureCoordinate).r;
    lowp float topColor = texture2D(jagged_textures, topTextureCoordinate).r;
    lowp float topRightColor = texture2D(jagged_textures, topRightTextureCoordinate).r;
    lowp float topLeftColor = texture2D(jagged_textures, topLeftTextureCoordinate).r;

    // Use a tiebreaker for pixels to the left and immediately above this one
    lowp float multiplier = 1.0 - step(centerColor.r, topColor);
    multiplier = multiplier * 1.0 - step(centerColor.r, topLeftColor);
    multiplier = multiplier * 1.0 - step(centerColor.r, leftColor);
    multiplier = multiplier * 1.0 - step(centerColor.r, bottomLeftColor);

    lowp float maxValue = max(centerColor.r, bottomColor);
    maxValue = max(maxValue, bottomRightColor);
    maxValue = max(maxValue, rightColor);
    maxValue = max(maxValue, topRightColor);

	centerColor.r = step(0.50, centerColor.r);
	centerColor.rgb = vec3(centerColor.r, centerColor.r, centerColor.r);
    gl_FragColor = vec4((centerColor.rgb * step(maxValue, centerColor.r) * multiplier), 1.0);
}
