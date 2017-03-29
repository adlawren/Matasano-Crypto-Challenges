#ifndef __ENGLISH_TRIGRAM_RELATIVE_FREQUENCY_PROVIDER_HPP__
#define __ENGLISH_TRIGRAM_RELATIVE_FREQUENCY_PROVIDER_HPP__

#include <map>

#include "ByteSequence.hpp"

class EnglishTrigramRelativeFrequencyProvider {
 public:
  static EnglishTrigramRelativeFrequencyProvider& getInstance() {
    static EnglishTrigramRelativeFrequencyProvider instance;
    return instance;
  }

  float getTrigramRelativeFrequency(
      const ByteSequence& trigramByteSequence) const {
    assert(trigramByteSequence.getByteCount() == 3);

    return _trigramRelativeFrequencyMap.at(trigramByteSequence.getBytes());
  }

  bool hasTrigramRelativeFrequency(
      const ByteSequence& trigramByteSequence) const {
    assert(trigramByteSequence.getByteCount() == 3);

    return _trigramRelativeFrequencyMap.find(trigramByteSequence.getBytes()) !=
           _trigramRelativeFrequencyMap.end();
  }

 private:
  static const std::map<std::vector<char>, float> _trigramRelativeFrequencyMap;

  EnglishTrigramRelativeFrequencyProvider() {
  }

  static const std::map<std::vector<char>, float>
  createTrigramRelativeFrequencyMap() {
    std::map<std::vector<char>, float> trigramRelativeFrequencyMap;

    trigramRelativeFrequencyMap[std::vector<char>{'t', 'h', 'e'}] =
        0.0181403609463;
    trigramRelativeFrequencyMap[std::vector<char>{'a', 'n', 'd'}] =
        0.0072522810875;
    trigramRelativeFrequencyMap[std::vector<char>{'i', 'n', 'g'}] =
        0.00717795270829;
    trigramRelativeFrequencyMap[std::vector<char>{'e', 'n', 't'}] =
        0.00418848180989;
    trigramRelativeFrequencyMap[std::vector<char>{'i', 'o', 'n'}] =
        0.00415740038163;
    trigramRelativeFrequencyMap[std::vector<char>{'h', 'e', 'r'}] =
        0.00357430061179;
    trigramRelativeFrequencyMap[std::vector<char>{'f', 'o', 'r'}] =
        0.00343605977937;
    trigramRelativeFrequencyMap[std::vector<char>{'t', 'h', 'a'}] =
        0.00332747950057;
    trigramRelativeFrequencyMap[std::vector<char>{'n', 't', 'h'}] =
        0.00330265080984;
    trigramRelativeFrequencyMap[std::vector<char>{'i', 'n', 't'}] =
        0.00319508383716;
    trigramRelativeFrequencyMap[std::vector<char>{'e', 'r', 'e'}] =
        0.00310874060929;
    trigramRelativeFrequencyMap[std::vector<char>{'t', 'i', 'o'}] =
        0.00310825162065;
    trigramRelativeFrequencyMap[std::vector<char>{'t', 'e', 'r'}] =
        0.00298770726377;
    trigramRelativeFrequencyMap[std::vector<char>{'e', 's', 't'}] =
        0.00279740481674;
    trigramRelativeFrequencyMap[std::vector<char>{'e', 'r', 's'}] =
        0.00276618230706;
    trigramRelativeFrequencyMap[std::vector<char>{'a', 't', 'i'}] =
        0.00262686874119;
    trigramRelativeFrequencyMap[std::vector<char>{'h', 'a', 't'}] =
        0.00255034061499;
    trigramRelativeFrequencyMap[std::vector<char>{'a', 't', 'e'}] =
        0.00250631198412;
    trigramRelativeFrequencyMap[std::vector<char>{'a', 'l', 'l'}] =
        0.00245690003284;
    trigramRelativeFrequencyMap[std::vector<char>{'e', 't', 'h'}] =
        0.00241080992881;
    trigramRelativeFrequencyMap[std::vector<char>{'h', 'e', 's'}] =
        0.00238398316965;
    trigramRelativeFrequencyMap[std::vector<char>{'v', 'e', 'r'}] =
        0.00237619000092;
    trigramRelativeFrequencyMap[std::vector<char>{'h', 'i', 's'}] =
        0.00235159995536;
    trigramRelativeFrequencyMap[std::vector<char>{'o', 'f', 't'}] =
        0.0022072914524;
    trigramRelativeFrequencyMap[std::vector<char>{'i', 't', 'h'}] =
        0.00213897225227;
    trigramRelativeFrequencyMap[std::vector<char>{'f', 't', 'h'}] =
        0.00211426779741;
    trigramRelativeFrequencyMap[std::vector<char>{'s', 't', 'h'}] =
        0.00211132146537;
    trigramRelativeFrequencyMap[std::vector<char>{'o', 't', 'h'}] =
        0.0020750567575;
    trigramRelativeFrequencyMap[std::vector<char>{'r', 'e', 's'}] =
        0.0020672921326;
    trigramRelativeFrequencyMap[std::vector<char>{'o', 'n', 't'}] =
        0.00204887667998;
    trigramRelativeFrequencyMap[std::vector<char>{'d', 't', 'h'}] =
        0.00204622912234;
    trigramRelativeFrequencyMap[std::vector<char>{'a', 'r', 'e'}] =
        0.00204513205644;
    trigramRelativeFrequencyMap[std::vector<char>{'r', 'e', 'a'}] =
        0.00203569714928;
    trigramRelativeFrequencyMap[std::vector<char>{'e', 'a', 'r'}] =
        0.00203502028605;
    trigramRelativeFrequencyMap[std::vector<char>{'w', 'a', 's'}] =
        0.00202168493409;
    trigramRelativeFrequencyMap[std::vector<char>{'s', 'i', 'n'}] =
        0.00201910031327;
    trigramRelativeFrequencyMap[std::vector<char>{'s', 't', 'o'}] =
        0.0020020077036;
    trigramRelativeFrequencyMap[std::vector<char>{'t', 't', 'h'}] =
        0.00198312244754;
    trigramRelativeFrequencyMap[std::vector<char>{'s', 't', 'a'}] =
        0.00196515995282;
    trigramRelativeFrequencyMap[std::vector<char>{'t', 'h', 'i'}] =
        0.00195679520549;
    trigramRelativeFrequencyMap[std::vector<char>{'t', 'i', 'n'}] =
        0.00192274241084;
    trigramRelativeFrequencyMap[std::vector<char>{'t', 'e', 'd'}] =
        0.00188145375413;
    trigramRelativeFrequencyMap[std::vector<char>{'o', 'n', 's'}] =
        0.00187675969713;
    trigramRelativeFrequencyMap[std::vector<char>{'e', 'd', 't'}] =
        0.00187646560205;
    trigramRelativeFrequencyMap[std::vector<char>{'w', 'i', 't'}] =
        0.00187283164436;
    trigramRelativeFrequencyMap[std::vector<char>{'s', 'a', 'n'}] =
        0.00184625429281;
    trigramRelativeFrequencyMap[std::vector<char>{'d', 'i', 'n'}] =
        0.00184262244081;
    trigramRelativeFrequencyMap[std::vector<char>{'o', 'r', 't'}] =
        0.00184239549392;
    trigramRelativeFrequencyMap[std::vector<char>{'c', 'o', 'n'}] =
        0.00182109383849;
    trigramRelativeFrequencyMap[std::vector<char>{'r', 't', 'h'}] =
        0.00178152763841;
    trigramRelativeFrequencyMap[std::vector<char>{'e', 'v', 'e'}] =
        0.0017539105426;
    trigramRelativeFrequencyMap[std::vector<char>{'e', 'c', 'o'}] =
        0.00173878886131;
    trigramRelativeFrequencyMap[std::vector<char>{'e', 'r', 'a'}] =
        0.00172123065959;
    trigramRelativeFrequencyMap[std::vector<char>{'i', 's', 't'}] =
        0.00171115562185;
    trigramRelativeFrequencyMap[std::vector<char>{'n', 'g', 't'}] =
        0.00167156532329;
    trigramRelativeFrequencyMap[std::vector<char>{'a', 's', 't'}] =
        0.00165564839206;
    trigramRelativeFrequencyMap[std::vector<char>{'i', 'l', 'l'}] =
        0.00163197642853;
    trigramRelativeFrequencyMap[std::vector<char>{'c', 'o', 'm'}] =
        0.00159617450513;
    trigramRelativeFrequencyMap[std::vector<char>{'o', 'r', 'e'}] =
        0.00157739125818;
    trigramRelativeFrequencyMap[std::vector<char>{'i', 'v', 'e'}] =
        0.00157060180297;
    trigramRelativeFrequencyMap[std::vector<char>{'n', 'c', 'e'}] =
        0.00150371143233;
    trigramRelativeFrequencyMap[std::vector<char>{'o', 'n', 'e'}] =
        0.00150344354142;
    trigramRelativeFrequencyMap[std::vector<char>{'e', 'd', 'i'}] =
        0.00149566815409;
    trigramRelativeFrequencyMap[std::vector<char>{'p', 'r', 'o'}] =
        0.00149535113035;
    trigramRelativeFrequencyMap[std::vector<char>{'e', 's', 's'}] =
        0.00149426038153;
    trigramRelativeFrequencyMap[std::vector<char>{'o', 'u', 't'}] =
        0.00149244340268;
    trigramRelativeFrequencyMap[std::vector<char>{'e', 'i', 'n'}] =
        0.0014471155594;
    trigramRelativeFrequencyMap[std::vector<char>{'a', 't', 't'}] =
        0.00144649812351;
    trigramRelativeFrequencyMap[std::vector<char>{'m', 'e', 'n'}] =
        0.00144598035707;
    trigramRelativeFrequencyMap[std::vector<char>{'h', 'e', 'c'}] =
        0.00143868459974;
    trigramRelativeFrequencyMap[std::vector<char>{'e', 's', 'a'}] =
        0.00142981752772;
    trigramRelativeFrequencyMap[std::vector<char>{'h', 'e', 'n'}] =
        0.00142828949671;
    trigramRelativeFrequencyMap[std::vector<char>{'i', 'n', 'a'}] =
        0.00142147126369;
    trigramRelativeFrequencyMap[std::vector<char>{'e', 'r', 'i'}] =
        0.00141551800246;
    trigramRelativeFrequencyMap[std::vector<char>{'e', 'r', 't'}] =
        0.00140290092568;
    trigramRelativeFrequencyMap[std::vector<char>{'a', 'm', 'e'}] =
        0.00137239926481;
    trigramRelativeFrequencyMap[std::vector<char>{'i', 't', 'i'}] =
        0.00137233118074;
    trigramRelativeFrequencyMap[std::vector<char>{'o', 'm', 'e'}] =
        0.00137115643817;
    trigramRelativeFrequencyMap[std::vector<char>{'s', 'o', 'n'}] =
        0.00136804165072;
    trigramRelativeFrequencyMap[std::vector<char>{'a', 'r', 't'}] =
        0.00136444231997;
    trigramRelativeFrequencyMap[std::vector<char>{'m', 'a', 'n'}] =
        0.00136105262263;
    trigramRelativeFrequencyMap[std::vector<char>{'e', 'a', 'n'}] =
        0.00135677385503;
    trigramRelativeFrequencyMap[std::vector<char>{'o', 'n', 'a'}] =
        0.00135230978648;
    trigramRelativeFrequencyMap[std::vector<char>{'e', 'o', 'f'}] =
        0.00134683755904;
    trigramRelativeFrequencyMap[std::vector<char>{'t', 'o', 'r'}] =
        0.00134482521871;
    trigramRelativeFrequencyMap[std::vector<char>{'h', 'e', 'a'}] =
        0.00133141944302;
    trigramRelativeFrequencyMap[std::vector<char>{'r', 'a', 'n'}] =
        0.00133119670752;
    trigramRelativeFrequencyMap[std::vector<char>{'r', 'i', 'n'}] =
        0.00131926304501;
    trigramRelativeFrequencyMap[std::vector<char>{'i', 'n', 'e'}] =
        0.00131807520036;
    trigramRelativeFrequencyMap[std::vector<char>{'e', 'd', 'a'}] =
        0.0013176671639;
    trigramRelativeFrequencyMap[std::vector<char>{'n', 't', 'o'}] =
        0.00130699785288;
    trigramRelativeFrequencyMap[std::vector<char>{'a', 'v', 'e'}] =
        0.0013038133436;
    trigramRelativeFrequencyMap[std::vector<char>{'n', 'i', 'n'}] =
        0.00129779901727;
    trigramRelativeFrequencyMap[std::vector<char>{'o', 'v', 'e'}] =
        0.00129425326471;
    trigramRelativeFrequencyMap[std::vector<char>{'o', 'u', 'n'}] =
        0.00128246044964;
    trigramRelativeFrequencyMap[std::vector<char>{'a', 'i', 'n'}] =
        0.00127105905945;
    trigramRelativeFrequencyMap[std::vector<char>{'a', 'n', 't'}] =
        0.00126856077203;
    trigramRelativeFrequencyMap[std::vector<char>{'s', 't', 'r'}] =
        0.00126259604647;
    trigramRelativeFrequencyMap[std::vector<char>{'e', 't', 'o'}] =
        0.00125780161812;
    trigramRelativeFrequencyMap[std::vector<char>{'h', 'e', 'm'}] =
        0.00125719728431;
    trigramRelativeFrequencyMap[std::vector<char>{'s', 'o', 'f'}] =
        0.00125594112163;
    trigramRelativeFrequencyMap[std::vector<char>{'p', 'e', 'r'}] =
        0.00123701211395;
    trigramRelativeFrequencyMap[std::vector<char>{'n', 'd', 'e'}] =
        0.001235764608;
    trigramRelativeFrequencyMap[std::vector<char>{'s', 't', 'e'}] =
        0.00123550584176;
    trigramRelativeFrequencyMap[std::vector<char>{'n', 't', 'e'}] =
        0.00122931533914;
    trigramRelativeFrequencyMap[std::vector<char>{'e', 'a', 's'}] =
        0.00122663993957;
    trigramRelativeFrequencyMap[std::vector<char>{'d', 't', 'o'}] =
        0.00122169530514;
    trigramRelativeFrequencyMap[std::vector<char>{'o', 'u', 'r'}] =
        0.00121715332596;
    trigramRelativeFrequencyMap[std::vector<char>{'r', 'e', 'd'}] =
        0.00120655560849;
    trigramRelativeFrequencyMap[std::vector<char>{'r', 'o', 'm'}] =
        0.00119335151137;
    trigramRelativeFrequencyMap[std::vector<char>{'t', 'o', 'f'}] =
        0.0011923412468;
    trigramRelativeFrequencyMap[std::vector<char>{'g', 'h', 't'}] =
        0.00119139649267;
    trigramRelativeFrequencyMap[std::vector<char>{'t', 'o', 't'}] =
        0.0011831307129;
    trigramRelativeFrequencyMap[std::vector<char>{'e', 's', 'e'}] =
        0.00118179383199;
    trigramRelativeFrequencyMap[std::vector<char>{'c', 'h', 'a'}] =
        0.00117958145085;
    trigramRelativeFrequencyMap[std::vector<char>{'i', 'c', 'a'}] =
        0.00117430832836;
    trigramRelativeFrequencyMap[std::vector<char>{'h', 'e', 'i'}] =
        0.00117330859225;
    trigramRelativeFrequencyMap[std::vector<char>{'h', 'i', 'n'}] =
        0.00117278801822;
    trigramRelativeFrequencyMap[std::vector<char>{'i', 'd', 'e'}] =
        0.0011692179332;
    trigramRelativeFrequencyMap[std::vector<char>{'n', 'd', 't'}] =
        0.00116704462435;
    trigramRelativeFrequencyMap[std::vector<char>{'h', 'a', 'n'}] =
        0.00116676175963;
    trigramRelativeFrequencyMap[std::vector<char>{'t', 'a', 'n'}] =
        0.00116671098904;
    trigramRelativeFrequencyMap[std::vector<char>{'l', 'i', 'n'}] =
        0.00116017491886;
    trigramRelativeFrequencyMap[std::vector<char>{'n', 'o', 't'}] =
        0.00115269198884;
    trigramRelativeFrequencyMap[std::vector<char>{'d', 'e', 'r'}] =
        0.00115266040345;
    trigramRelativeFrequencyMap[std::vector<char>{'e', 'c', 't'}] =
        0.00114287969476;
    trigramRelativeFrequencyMap[std::vector<char>{'t', 'r', 'a'}] =
        0.00114092350623;
    trigramRelativeFrequencyMap[std::vector<char>{'i', 'g', 'h'}] =
        0.00113799776318;
    trigramRelativeFrequencyMap[std::vector<char>{'f', 'r', 'o'}] =
        0.00113003239557;
    trigramRelativeFrequencyMap[std::vector<char>{'e', 'a', 't'}] =
        0.0011299589303;
    trigramRelativeFrequencyMap[std::vector<char>{'s', 't', 'i'}] =
        0.00112517737007;
    trigramRelativeFrequencyMap[std::vector<char>{'h', 'e', 'p'}] =
        0.00110760910782;
    trigramRelativeFrequencyMap[std::vector<char>{'n', 'd', 'i'}] =
        0.00110756114482;
    trigramRelativeFrequencyMap[std::vector<char>{'i', 'n', 's'}] =
        0.00110693388236;
    trigramRelativeFrequencyMap[std::vector<char>{'s', 'h', 'e'}] =
        0.0010986365172;
    trigramRelativeFrequencyMap[std::vector<char>{'n', 'a', 'l'}] =
        0.00107360825359;
    trigramRelativeFrequencyMap[std::vector<char>{'p', 'l', 'a'}] =
        0.00106910441084;
    trigramRelativeFrequencyMap[std::vector<char>{'a', 'l', 's'}] =
        0.00106120244798;
    trigramRelativeFrequencyMap[std::vector<char>{'e', 'e', 'n'}] =
        0.00105832770949;
    trigramRelativeFrequencyMap[std::vector<char>{'n', 't', 'i'}] =
        0.00105694848076;
    trigramRelativeFrequencyMap[std::vector<char>{'y', 'o', 'u'}] =
        0.00105227009948;
    trigramRelativeFrequencyMap[std::vector<char>{'l', 'a', 'n'}] =
        0.00105215405242;
    trigramRelativeFrequencyMap[std::vector<char>{'u', 'n', 'd'}] =
        0.00104308974718;
    trigramRelativeFrequencyMap[std::vector<char>{'n', 'd', 'a'}] =
        0.0010424844775;
    trigramRelativeFrequencyMap[std::vector<char>{'r', 'a', 't'}] =
        0.00103442435372;
    trigramRelativeFrequencyMap[std::vector<char>{'l', 'e', 'a'}] =
        0.00103218880996;
    trigramRelativeFrequencyMap[std::vector<char>{'c', 'a', 'n'}] =
        0.00103199181071;
    trigramRelativeFrequencyMap[std::vector<char>{'h', 'a', 's'}] =
        0.00102493657028;
    trigramRelativeFrequencyMap[std::vector<char>{'n', 'd', 's'}] =
        0.00102233182839;
    trigramRelativeFrequencyMap[std::vector<char>{'n', 'g', 'a'}] =
        0.001018120209;
    trigramRelativeFrequencyMap[std::vector<char>{'h', 'e', 'l'}] =
        0.00101529460334;
    trigramRelativeFrequencyMap[std::vector<char>{'h', 'e', 'd'}] =
        0.00100960408576;
    trigramRelativeFrequencyMap[std::vector<char>{'i', 'n', 'c'}] =
        0.00100527665327;
    trigramRelativeFrequencyMap[std::vector<char>{'u', 's', 'e'}] =
        0.000990756030273;
    trigramRelativeFrequencyMap[std::vector<char>{'e', 's', 'i'}] =
        0.000989034743462;
    trigramRelativeFrequencyMap[std::vector<char>{'g', 't', 'h'}] =
        0.00098891003966;
    trigramRelativeFrequencyMap[std::vector<char>{'a', 's', 'a'}] =
        0.000986905420195;
    trigramRelativeFrequencyMap[std::vector<char>{'h', 'e', 't'}] =
        0.000985711726392;
    trigramRelativeFrequencyMap[std::vector<char>{'n', 't', 's'}] =
        0.000982457261318;
    trigramRelativeFrequencyMap[std::vector<char>{'h', 'a', 'v'}] =
        0.000979168637229;
    trigramRelativeFrequencyMap[std::vector<char>{'h', 'e', 'w'}] =
        0.00097894777346;
    trigramRelativeFrequencyMap[std::vector<char>{'t', 'h', 'o'}] =
        0.000978188086322;
    trigramRelativeFrequencyMap[std::vector<char>{'b', 'u', 't'}] =
        0.000977377862558;
    trigramRelativeFrequencyMap[std::vector<char>{'n', 'a', 'n'}] =
        0.000975440859227;
    trigramRelativeFrequencyMap[std::vector<char>{'a', 's', 's'}] =
        0.000974103042455;
    trigramRelativeFrequencyMap[std::vector<char>{'h', 'e', 'f'}] =
        0.000966417264046;
    trigramRelativeFrequencyMap[std::vector<char>{'i', 'e', 's'}] =
        0.000964454290504;
    trigramRelativeFrequencyMap[std::vector<char>{'r', 'e', 't'}] =
        0.000963853232217;
    trigramRelativeFrequencyMap[std::vector<char>{'e', 'n', 'd'}] =
        0.00095606965608;
    trigramRelativeFrequencyMap[std::vector<char>{'p', 'a', 'r'}] =
        0.00095420541613;
    trigramRelativeFrequencyMap[std::vector<char>{'w', 'e', 'r'}] =
        0.000947410579705;
    trigramRelativeFrequencyMap[std::vector<char>{'c', 't', 'i'}] =
        0.000945398005402;
    trigramRelativeFrequencyMap[std::vector<char>{'r', 'e', 'n'}] =
        0.000942480217197;
    trigramRelativeFrequencyMap[std::vector<char>{'r', 'e', 'c'}] =
        0.000937547515029;
    trigramRelativeFrequencyMap[std::vector<char>{'c', 'a', 'l'}] =
        0.000934301238761;
    trigramRelativeFrequencyMap[std::vector<char>{'i', 't', 's'}] =
        0.000933933210467;
    trigramRelativeFrequencyMap[std::vector<char>{'r', 'e', 'e'}] =
        0.000928344000105;
    trigramRelativeFrequencyMap[std::vector<char>{'e', 'n', 'e'}] =
        0.000927552961541;
    trigramRelativeFrequencyMap[std::vector<char>{'r', 's', 't'}] =
        0.00092602259087;
    trigramRelativeFrequencyMap[std::vector<char>{'e', 'a', 'l'}] =
        0.000925009518708;
    trigramRelativeFrequencyMap[std::vector<char>{'a', 'n', 'a'}] =
        0.000919041049691;
    trigramRelativeFrequencyMap[std::vector<char>{'n', 's', 't'}] =
        0.000916636348611;
    trigramRelativeFrequencyMap[std::vector<char>{'c', 'o', 'u'}] =
        0.000913584731935;
    trigramRelativeFrequencyMap[std::vector<char>{'t', 'u', 'r'}] =
        0.000912048278151;
    trigramRelativeFrequencyMap[std::vector<char>{'m', 'i', 'n'}] =
        0.000905973120703;
    trigramRelativeFrequencyMap[std::vector<char>{'i', 't', 'y'}] =
        0.000904847276998;
    trigramRelativeFrequencyMap[std::vector<char>{'y', 't', 'h'}] =
        0.000904078231225;
    trigramRelativeFrequencyMap[std::vector<char>{'h', 'e', 'y'}] =
        0.000903110314474;
    trigramRelativeFrequencyMap[std::vector<char>{'e', 'c', 'a'}] =
        0.000899208231908;
    trigramRelativeFrequencyMap[std::vector<char>{'o', 'u', 'l'}] =
        0.000898764164711;
    trigramRelativeFrequencyMap[std::vector<char>{'l', 'l', 'e'}] =
        0.000892537631353;
    trigramRelativeFrequencyMap[std::vector<char>{'a', 'r', 'd'}] =
        0.000889325982032;
    trigramRelativeFrequencyMap[std::vector<char>{'r', 'o', 'u'}] =
        0.000889092484106;
    trigramRelativeFrequencyMap[std::vector<char>{'a', 'n', 'c'}] =
        0.000888120122004;
    trigramRelativeFrequencyMap[std::vector<char>{'o', 's', 't'}] =
        0.000887518127853;
    trigramRelativeFrequencyMap[std::vector<char>{'p', 'r', 'e'}] =
        0.000878921754328;
    trigramRelativeFrequencyMap[std::vector<char>{'a', 'g', 'e'}] =
        0.000878314612928;
    trigramRelativeFrequencyMap[std::vector<char>{'e', 'f', 'o'}] =
        0.000877039031075;
    trigramRelativeFrequencyMap[std::vector<char>{'l', 'e', 's'}] =
        0.000875883941638;
    trigramRelativeFrequencyMap[std::vector<char>{'s', 's', 'i'}] =
        0.000871746957351;
    trigramRelativeFrequencyMap[std::vector<char>{'e', 'm', 'a'}] =
        0.000869923193494;
    trigramRelativeFrequencyMap[std::vector<char>{'e', 's', 'o'}] =
        0.000869804572805;
    trigramRelativeFrequencyMap[std::vector<char>{'t', 'a', 't'}] =
        0.000868738390393;
    trigramRelativeFrequencyMap[std::vector<char>{'a', 't', 'h'}] =
        0.000867799017477;
    trigramRelativeFrequencyMap[std::vector<char>{'w', 'o', 'r'}] =
        0.000864056031693;
    trigramRelativeFrequencyMap[std::vector<char>{'u', 's', 't'}] =
        0.000862929486091;
    trigramRelativeFrequencyMap[std::vector<char>{'h', 'e', 'b'}] =
        0.000862483079235;
    trigramRelativeFrequencyMap[std::vector<char>{'e', 'w', 'a'}] =
        0.000861415493029;
    trigramRelativeFrequencyMap[std::vector<char>{'s', 'h', 'o'}] =
        0.000858990904851;
    trigramRelativeFrequencyMap[std::vector<char>{'i', 'n', 'd'}] =
        0.000855096777124;
    trigramRelativeFrequencyMap[std::vector<char>{'s', 'e', 'd'}] =
        0.000844965353609;
    trigramRelativeFrequencyMap[std::vector<char>{'h', 'o', 'u'}] =
        0.000840615928324;
    trigramRelativeFrequencyMap[std::vector<char>{'l', 'l', 'y'}] =
        0.000840494266078;
    trigramRelativeFrequencyMap[std::vector<char>{'u', 'l', 'd'}] =
        0.000839367018578;
    trigramRelativeFrequencyMap[std::vector<char>{'a', 's', 'e'}] =
        0.000836406882553;
    trigramRelativeFrequencyMap[std::vector<char>{'u', 'r', 'e'}] =
        0.000836381614241;
    trigramRelativeFrequencyMap[std::vector<char>{'o', 'n', 'o'}] =
        0.000834740811684;
    trigramRelativeFrequencyMap[std::vector<char>{'e', 'l', 'e'}] =
        0.000833659889424;
    trigramRelativeFrequencyMap[std::vector<char>{'e', 'n', 'c'}] =
        0.000829295022392;
    trigramRelativeFrequencyMap[std::vector<char>{'n', 'a', 't'}] =
        0.000826908804614;
    trigramRelativeFrequencyMap[std::vector<char>{'e', 'a', 'd'}] =
        0.000825580814409;
    trigramRelativeFrequencyMap[std::vector<char>{'w', 'h', 'e'}] =
        0.000823965514131;
    trigramRelativeFrequencyMap[std::vector<char>{'e', 'l', 'l'}] =
        0.000823250046539;
    trigramRelativeFrequencyMap[std::vector<char>{'b', 'l', 'e'}] =
        0.000820526683962;
    trigramRelativeFrequencyMap[std::vector<char>{'k', 'i', 'n'}] =
        0.000819292514065;
    trigramRelativeFrequencyMap[std::vector<char>{'a', 'n', 's'}] =
        0.000818671802646;
    trigramRelativeFrequencyMap[std::vector<char>{'t', 'i', 'c'}] =
        0.000816289094356;
    trigramRelativeFrequencyMap[std::vector<char>{'a', 'l', 'i'}] =
        0.000815324687093;
    trigramRelativeFrequencyMap[std::vector<char>{'s', 'c', 'o'}] =
        0.00081489278612;
    trigramRelativeFrequencyMap[std::vector<char>{'e', 'r', 'o'}] =
        0.000813921125915;
    trigramRelativeFrequencyMap[std::vector<char>{'w', 'h', 'i'}] =
        0.000813685990229;
    trigramRelativeFrequencyMap[std::vector<char>{'c', 'e', 's'}] =
        0.000810061625135;
    trigramRelativeFrequencyMap[std::vector<char>{'o', 'w', 'n'}] =
        0.000806759664993;
    trigramRelativeFrequencyMap[std::vector<char>{'n', 't', 'a'}] =
        0.000806037646357;
    trigramRelativeFrequencyMap[std::vector<char>{'a', 'c', 't'}] =
        0.000803848895763;
    trigramRelativeFrequencyMap[std::vector<char>{'b', 'e', 'r'}] =
        0.000803443198967;
    trigramRelativeFrequencyMap[std::vector<char>{'v', 'e', 'n'}] =
        0.00080011269499;
    trigramRelativeFrequencyMap[std::vector<char>{'t', 'i', 'm'}] =
        0.000795021597937;
    trigramRelativeFrequencyMap[std::vector<char>{'d', 'o', 'n'}] =
        0.000790637311739;
    trigramRelativeFrequencyMap[std::vector<char>{'d', 'a', 'n'}] =
        0.000788974516392;
    trigramRelativeFrequencyMap[std::vector<char>{'o', 's', 'e'}] =
        0.000788876016766;
    trigramRelativeFrequencyMap[std::vector<char>{'i', 'c', 'e'}] =
        0.000788790385262;
    trigramRelativeFrequencyMap[std::vector<char>{'i', 's', 'a'}] =
        0.000788561098722;
    trigramRelativeFrequencyMap[std::vector<char>{'t', 'o', 'n'}] =
        0.000784563090154;
    trigramRelativeFrequencyMap[std::vector<char>{'d', 'e', 'n'}] =
        0.000782522673914;
    trigramRelativeFrequencyMap[std::vector<char>{'n', 'g', 's'}] =
        0.000779313832182;
    trigramRelativeFrequencyMap[std::vector<char>{'u', 'g', 'h'}] =
        0.000776237415126;
    trigramRelativeFrequencyMap[std::vector<char>{'n', 'e', 's'}] =
        0.000774420202313;
    trigramRelativeFrequencyMap[std::vector<char>{'l', 'a', 't'}] =
        0.000772429854766;
    trigramRelativeFrequencyMap[std::vector<char>{'t', 'a', 'l'}] =
        0.000769993334329;
    trigramRelativeFrequencyMap[std::vector<char>{'e', 'd', 'o'}] =
        0.000769378472056;
    trigramRelativeFrequencyMap[std::vector<char>{'t', 'e', 'n'}] =
        0.000768188053775;
    trigramRelativeFrequencyMap[std::vector<char>{'i', 'm', 'e'}] =
        0.000767561259244;
    trigramRelativeFrequencyMap[std::vector<char>{'e', 'm', 'e'}] =
        0.00076503138643;
    trigramRelativeFrequencyMap[std::vector<char>{'a', 'c', 'k'}] =
        0.000764364349771;
    trigramRelativeFrequencyMap[std::vector<char>{'t', 'e', 's'}] =
        0.000763019280058;
    trigramRelativeFrequencyMap[std::vector<char>{'p', 'l', 'e'}] =
        0.00076160729611;
    trigramRelativeFrequencyMap[std::vector<char>{'o', 'u', 's'}] =
        0.000759182006034;
    trigramRelativeFrequencyMap[std::vector<char>{'o', 'f', 'f'}] =
        0.000758236316039;
    trigramRelativeFrequencyMap[std::vector<char>{'t', 't', 'o'}] =
        0.000751695332569;
    trigramRelativeFrequencyMap[std::vector<char>{'c', 'h', 'i'}] =
        0.000750041427925;
    trigramRelativeFrequencyMap[std::vector<char>{'a', 'n', 'i'}] =
        0.000749880693381;
    trigramRelativeFrequencyMap[std::vector<char>{'o', 'r', 'm'}] =
        0.000748566741127;
    trigramRelativeFrequencyMap[std::vector<char>{'n', 'e', 'd'}] =
        0.000745138673387;
    trigramRelativeFrequencyMap[std::vector<char>{'e', 'n', 's'}] =
        0.000744576687398;
    trigramRelativeFrequencyMap[std::vector<char>{'s', 'h', 'a'}] =
        0.000742499070586;
    trigramRelativeFrequencyMap[std::vector<char>{'m', 'o', 'r'}] =
        0.000740908336723;
    trigramRelativeFrequencyMap[std::vector<char>{'i', 's', 's'}] =
        0.00074058944126;
    trigramRelativeFrequencyMap[std::vector<char>{'i', 't', 'e'}] =
        0.000738615003391;
    trigramRelativeFrequencyMap[std::vector<char>{'n', 'g', 'e'}] =
        0.000737298711479;
    trigramRelativeFrequencyMap[std::vector<char>{'t', 'i', 's'}] =
        0.000736259435141;
    trigramRelativeFrequencyMap[std::vector<char>{'o', 'r', 'a'}] =
        0.000735436577221;
    trigramRelativeFrequencyMap[std::vector<char>{'l', 'l', 'i'}] =
        0.000733859413378;
    trigramRelativeFrequencyMap[std::vector<char>{'e', 'd', 'e'}] =
        0.000732304008359;
    trigramRelativeFrequencyMap[std::vector<char>{'s', 's', 'e'}] =
        0.000731655221037;
    trigramRelativeFrequencyMap[std::vector<char>{'a', 'd', 'e'}] =
        0.00073088360164;
    trigramRelativeFrequencyMap[std::vector<char>{'r', 'i', 'e'}] =
        0.000728508848189;
    trigramRelativeFrequencyMap[std::vector<char>{'a', 'i', 'd'}] =
        0.000728437020671;
    trigramRelativeFrequencyMap[std::vector<char>{'e', 'm', 'o'}] =
        0.000727659786094;
    trigramRelativeFrequencyMap[std::vector<char>{'r', 'a', 'l'}] =
        0.000727086569744;
    trigramRelativeFrequencyMap[std::vector<char>{'s', 'i', 't'}] =
        0.000725686050122;
    trigramRelativeFrequencyMap[std::vector<char>{'o', 'i', 'n'}] =
        0.000722653150716;
    trigramRelativeFrequencyMap[std::vector<char>{'h', 't', 'h'}] =
        0.000716042211455;
    trigramRelativeFrequencyMap[std::vector<char>{'t', 'r', 'e'}] =
        0.000714152937158;
    trigramRelativeFrequencyMap[std::vector<char>{'a', 'n', 'y'}] =
        0.000713720568254;
    trigramRelativeFrequencyMap[std::vector<char>{'a', 'k', 'e'}] =
        0.000712322154325;
    trigramRelativeFrequencyMap[std::vector<char>{'e', 'r', 'n'}] =
        0.000711641313681;
    trigramRelativeFrequencyMap[std::vector<char>{'m', 'e', 'r'}] =
        0.000710212016259;
    trigramRelativeFrequencyMap[std::vector<char>{'r', 'i', 'c'}] =
        0.000708400886558;
    trigramRelativeFrequencyMap[std::vector<char>{'d', 'i', 's'}] =
        0.000708359708568;
    trigramRelativeFrequencyMap[std::vector<char>{'i', 's', 'h'}] =
        0.000707627863366;
    trigramRelativeFrequencyMap[std::vector<char>{'o', 'u', 'g'}] =
        0.000707613357483;
    trigramRelativeFrequencyMap[std::vector<char>{'i', 'n', 'i'}] =
        0.000706528925735;
    trigramRelativeFrequencyMap[std::vector<char>{'o', 'n', 'g'}] =
        0.000704900055437;
    trigramRelativeFrequencyMap[std::vector<char>{'n', 't', 'r'}] =
        0.00070472481501;
    trigramRelativeFrequencyMap[std::vector<char>{'e', 'l', 'i'}] =
        0.000703868733939;
    trigramRelativeFrequencyMap[std::vector<char>{'w', 'i', 'l'}] =
        0.000702078193234;
    trigramRelativeFrequencyMap[std::vector<char>{'l', 'e', 'd'}] =
        0.000701717651848;
    trigramRelativeFrequencyMap[std::vector<char>{'s', 'a', 'r'}] =
        0.000701611197383;
    trigramRelativeFrequencyMap[std::vector<char>{'h', 'o', 'w'}] =
        0.000698232262473;

    return trigramRelativeFrequencyMap;
  }
};

const std::map<std::vector<char>, float>
    EnglishTrigramRelativeFrequencyProvider::_trigramRelativeFrequencyMap =
        EnglishTrigramRelativeFrequencyProvider::
            createTrigramRelativeFrequencyMap();

#endif  // __ENGLISH_TRIGRAM_RELATIVE_FREQUENCY_PROVIDER_HPP__
