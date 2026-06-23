struct VertexShaderOutput
{
    float32_t4 position : SV_POSITION;
    float32_t2 texcoord : TEXCOORD0;
    float32_t3 normal : NORMAL0;
    float32_t3 worldPosition : POSITION0;
};

struct MagicCircleConfig
{
    float4 color;
    float DrawThreshold; // 描画具合 (0.0 ～ 1.0)
    float DissolveThreshold; // 消失具合 (0.0 ～ 1.0)
    float RotationAngle; // 回転角度 (ラジアン)
    float Padding; // 16バイトアライメント用
};

ConstantBuffer<MagicCircleConfig> gMagic : register(b0);
Texture2D<float4> gTexture : register(t0);
Texture2D<float> gRevealTexture : register(t1);
Texture2D<float> gNoiseTexture : register(t2);
SamplerState gSampler : register(s0);

float4 main(VertexShaderOutput input) : SV_TARGET
{
	// ----------------------------------------------------
    // ① UVの回転処理（魔法陣を回す！）
    // ----------------------------------------------------
    // UVの中心(0.5, 0.5)を基準に回転させる
    float s, c;
    sincos(gMagic.RotationAngle, s, c);
    float2 centeredUV = input.texcoord - 0.5f;
    float2 rotatedUV = float2(
        centeredUV.x * c - centeredUV.y * s,
        centeredUV.x * s + centeredUV.y * c
    ) + 0.5f;

    // ----------------------------------------------------
    // ② テクスチャのサンプリング
    // ----------------------------------------------------
    // 魔法陣は回転したUVで、グラデーションはそのままのUVで読み込む
    // （筆の動きは固定で、下敷きの魔法陣だけが回るような表現になる）
    float baseAlpha = gTexture.Sample(gSampler, rotatedUV).r;
    float revealVal = gRevealTexture.Sample(gSampler, input.texcoord).r;
    float noiseVal = gNoiseTexture.Sample(gSampler, rotatedUV).r;

    // 模様がない部分はそもそも描画しない
    if (baseAlpha <= 0.01f)
        discard;

    // ----------------------------------------------------
    // ③ 描画処理 ＆ オーバードライブ（筆の先端発光）
    // ----------------------------------------------------
    // グラデーション値が閾値より大きい部分はまだ「描かれていない」ので捨てる
    if (revealVal > gMagic.DrawThreshold)
        discard;

    // ベースとなる色（頂点カラーなどを乗算）
    float4 finalColor = gMagic.color * baseAlpha;

    // オーバードライブの計算（先端のわずかな領域だけ爆発的に光らせる）
    float edgeWidth = 0.05f; // 先端の幅
    if (gMagic.DrawThreshold < 1.0f && revealVal > (gMagic.DrawThreshold - edgeWidth))
    {
        // 5.0倍してHDRの白飛び領域へ持っていく（ブルーム効果と合わさって輝く！）
        finalColor.rgb *= 5.0f;
    }

    // ----------------------------------------------------
    // ④ ディゾルブ処理（シュワァァっと消滅）
    // ----------------------------------------------------
    // ノイズ値が消失閾値より小さくなったら捨てる
    if (noiseVal < gMagic.DissolveThreshold)
        discard;

    // 消滅していくエッジ部分も少し発光させる
    if (gMagic.DissolveThreshold > 0.0f && noiseVal < (gMagic.DissolveThreshold + 0.05f))
    {
        // ここは消滅時のエネルギーの色（例としてオレンジっぽく光らせる）
        finalColor.rgb = float3(1.0f, 0.5f, 0.1f) * 3.0f;
    }

    return finalColor;
}