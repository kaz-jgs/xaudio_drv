���ޗpXAudio2�T�E���h�h���C�over 0.0.0 by kawatanik

���T�v
�T�E���h�v���O���~���O�̓�����ɗ����߂̋��ޗp�Ƃ��č�����A�V���v���ȃT�E���h�h���C�o�ł��B
Windows��ł�XAudio2��p�����h���C�o�ł��B���������ē������Windows XP�ȍ~�ADirectX 9�ȍ~�ł��B
�ق��̊��͑z�肵�Ă��܂���Xbox�n�Ȃ瓮����������܂���B�������T�|�[�g�ΏۊO�ł��B

���L�̖����ꍇ��C++11 + STL�ŏ�����Ă��܂��B
Visual Studio�Ńr���h���邱�Ƃ����z�肵�Ă��Ȃ����߁A�ق��̃r���h�V�X�e���ł̎g�p�̓T�|�[�g�ΏۊO�ł��B
�Ȃ��AC++11���g���Ă���̂�Visual Studio��2013�ȍ~�݂̂̑Ή��ł��B

�����܂ł����ޗp�ł�����p�͍l�����Ă��Ȃ��̂ŁA�`���[�g���A���p�r�ȊO�̎g�p�͔񐄏��ł��B�T�|�[�g�����܂���B
���ɁA�N���X�\�����͎��p�����ǂ݂₷����T�E���h�v���O�����̏����̂킩��₷�����d�����Ă���̂ŁA
���p������Ȃ狁�߂���X�y�b�N��c�[���쐬���l��������ŃN���X�݌v�����蒼�����ق��������ł��B
���ƃ��������f�Љ�������Ȃ̂ł��̂܂܂ɋ߂��`�Ŏg���Ȃ点�߂ăA���P�[�^�𓱓������ق��������Ǝv���܂��B


��ver 0.0.0�ɂ���
�t�@�C����H�킹���特���邾���ł��B�I�u�W�F�N�g�Ǘ����炠��܂���B
����N���X�\�����K���b�ƕς��\��������܂��B


���t�@�C�� / �N���X
util/macros_debug.h
�f�o�b�O�}�N�����`���Ă邾���ł��B���܂�C�ɂ��Ȃ��Ă����ł��B


Drv.h / Drv.cpp
�T�E���h�h���C�o�{�́B�O���[�o���֐��łł��Ă��܂��B


class Core(Core.h / Core.cpp)
XAudio2�̃G���W���Ǘ�����у}�X�^�����O�{�C�X�Ǘ��̃N���X�ł��B
�V���O���g���ł��B


class Master(Master.h / Master.cpp)
�}�X�^�[�g���b�N�N���X�\��n�ł�(������)�B


class Bus(Bus.h / Bus.cpp)
�o�̓o�X�N���X�\��n�ł�(������)�B


class Snd(Snd.h / Snd.cpp)
�����I�u�W�F�N�g�N���X�ł��B
�I���������Đ��݂̂ł��B
�X�g���[�~���O�͎q�N���X�ɂ��邩�������邩�Y�ݒ��ł��B


class SndCallback(SndCallback.h / SndCallback.cpp)
�����I�u�W�F�N�g�p�R�[���o�b�N�N���X�ł��B


class TrackBase(TrackBase.cpp / TrackBase.h)
�g���b�N�̊��N���X�ł��B
Master�ABus�ASnd�͂��̃N���X���p�����܂��B
�`�����l���X�g���b�v�ɕK�v�ȋ@�\�͂����ɏ��\��ł��B


class Wave(Wave.cpp / Wave.h)
Wave�t�@�C���A�N�Z�T�ł��B
�w��t�@�C������Wave�t�@�C����ǂ�ŁA�g�`�o�b�t�@��t�H�[�}�b�g��Ԃ��Ă���܂��B
�P�Ȃ�t�@�C���p�[�T�Ȃ̂ŁA�T�E���h�h���C�o�̗����Ƃ����Ӗ��ł͂��̃N���X�͓ǂ܂Ȃ��Ă������ł��B
���Ȃ݂ɓ����Ŗ����������}�b�v�g�t�@�C���Ƃ��ă}�b�s���O���Ă���̂ŁA
�I���������Đ����W���X�g�C���^�C�����[�h�ŋ^���X�g���[�~���O�ɂł��܂��B



������̎����\��
 - Bus�N���X�����Master�N���X�̎���
 - �I�u�W�F�N�g�Ǘ��̎���
 - Wave�t�@�C����smpl�`�����N�̃p�[�X��smpl�`�����N��p�������[�v����
 - Wave�t�@�C���̔񈳏k�ȊO�̃t�H�[�}�b�g�Ή�
 - �X�g���[�~���O�Đ��̎���
 - �G�t�F�N�g�`�F�C���̎���
 - �G�t�F�N�g�C���^�[�t�F�C�X���b�p�̎���