#include "MagicCircle.hlsli"

struct PixelShaderOutPut
{
    float4 color : SV_TARGET0;
};

struct MagicCircleConfig
{
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

//PixelShaderOutPut main(VertexShaderOutput input)
//{
//	// ----------------------------------------------------
//    // ① UVの回転処理（魔法陣を回す！）
//    // ----------------------------------------------------
//    // UVの中心(0.5, 0.5)を基準に回転させる
//    float s, c;
//    sincos(gMagic.RotationAngle, s, c);
//    float2 centeredUV = input.texcoord - 0.5f;
//    float2 rotatedUV = float2(
//        centeredUV.x * c - centeredUV.y * s,
//        centeredUV.x * s + centeredUV.y * c
//    ) + 0.5f;

//    // ----------------------------------------------------
//    // ② テクスチャのサンプリング
//    // ----------------------------------------------------
//    // 魔法陣は回転したUVで、グラデーションはそのままのUVで読み込む
//    // （筆の動きは固定で、下敷きの魔法陣だけが回るような表現になる）
//    float baseAlpha = gTexture.Sample(gSampler, rotatedUV).r;
//    float revealVal = gRevealTexture.Sample(gSampler, input.texcoord).r;
//    float noiseVal = gNoiseTexture.Sample(gSampler, rotatedUV).r;

//    // 模様がない部分はそもそも描画しない
//    //if (baseAlpha <= 0.01f)
//    //    discard;

//    // ----------------------------------------------------
//    // ③ 描画処理 ＆ オーバードライブ（筆の先端発光）
//    // ----------------------------------------------------
//    // グラデーション値が閾値より大きい部分はまだ「描かれていない」ので捨てる
//    if (revealVal > gMagic.DrawThreshold)
//        discard;

//    PixelShaderOutPut output;
    
//    // ベースとなる色（頂点カラーなどを乗算）
//    output.color = input.color * baseAlpha;

//    // オーバードライブの計算（先端のわずかな領域だけ爆発的に光らせる）
//    float edgeWidth = 0.05f; // 先端の幅
//    if (gMagic.DrawThreshold < 1.0f && revealVal > (gMagic.DrawThreshold - edgeWidth))
//    {
//        // 5.0倍してHDRの白飛び領域へ持っていく（ブルーム効果と合わさって輝く！）
//        output.color.rgb *= 5.0f;
//    }

//    // ----------------------------------------------------
//    // ④ ディゾルブ処理（シュワァァっと消滅）
//    // ----------------------------------------------------
//    // ノイズ値が消失閾値より小さくなったら捨てる
//    if (noiseVal < gMagic.DissolveThreshold)
//        discard;

//    // 消滅していくエッジ部分も少し発光させる
//    if (gMagic.DissolveThreshold > 0.0f && noiseVal < (gMagic.DissolveThreshold + 0.05f))
//    {
//        // ここは消滅時のエネルギーの色（例としてオレンジっぽく光らせる）
//        output.color.rgb = float3(1.0f, 0.5f, 0.1f) * 3.0f;
//    }

//    return output;
//}

PixelShaderOutPut main(VertexShaderOutput input)
{
    // ----------------------------------------------------
    // ① UVの回転処理（魔法陣を回す）
    // ----------------------------------------------------
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
    // 今回はRGBの色もちゃんと使うので float4 で受けるよ
    float4 baseColor = gTexture.Sample(gSampler, rotatedUV);
    float revealVal = gRevealTexture.Sample(gSampler, input.texcoord).r;
    float noiseVal = gNoiseTexture.Sample(gSampler, rotatedUV).r;

    PixelShaderOutPut output;

    // ----------------------------------------------------
    // ③ 描画処理 ＆ オーバードライブ（光の筆）
    // ----------------------------------------------------
    // ベースとなる魔法陣の色（頂点カラーなどを乗算）
    float4 finalColor = baseColor * input.color;
    
    // 【ここがポイント！】
    // 筆が通る前の「暗い状態」の色を作る（元の明るさの10%にしておく）
    float3 dimColor = finalColor.rgb * 0.0f;
    float3 brightColor = finalColor.rgb;
    
    float3 currentDrawingColor = dimColor; // 初期状態は暗い色
    float edgeWidth = 0.05f; // 筆の先端の幅

    // グラデーション値が閾値以下＝「すでに筆が通った場所」
    if (revealVal <= gMagic.DrawThreshold)
    {
        currentDrawingColor = brightColor; // 通常の明るさに

        // さらに、筆の最前線（先端）だけ爆発的に光らせる（オーバードライブ）
        if (gMagic.DrawThreshold < 1.0f && revealVal > (gMagic.DrawThreshold - edgeWidth))
        {
            currentDrawingColor *= 5.0f; // 5倍の明るさでHDR発光！
        }
    }

    // 一旦、描画フェーズの色をセット（アルファは1.0で不透明にする）
    output.color = float4(currentDrawingColor, 1.0f);

    // ----------------------------------------------------
    // ④ ディゾルブ処理（シュワァァっと消滅）
    // ----------------------------------------------------
    // 消滅が始まっている場合（DissolveThreshold > 0 のとき）
    if (gMagic.DissolveThreshold > 0.0f)
    {
        // ノイズ値が消失閾値より小さくなったら「完全に消滅」
        if (noiseVal < gMagic.DissolveThreshold)
        {
            // ここを discard ではなく「真っ黒」にすることで、黒いポリゴンとして残るよ
            output.color.rgb = float3(0.0f, 0.0f, 0.0f);
            //output.color.a = 0.0f;
        }
        // 消滅していくエッジ（境界線）をオレンジ色に光らせる
        else if (noiseVal < (gMagic.DissolveThreshold + 0.05f))
        {
            output.color.rgb = float3(1.0f, 0.5f, 0.1f) * 3.0f;
        }
    }

    return output;
}