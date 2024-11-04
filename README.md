# **LLM 기반 FPS Defense 게임 : Lucid**

## 📋 프로젝트 개요
> **LLM 기반 FPS Defense 게임 Lucid**는 4인 멀티플레이어를 지원하는 3D FPS 생존 게임입니다. AI 기반 음성 인식 인터페이스와 커스텀 보스 생성 기능을 통해 창의적이고 몰입감 있는 플레이 경험을 제공하는 것을 목표로 합니다.

### 🎯 주요 기능
- **🎙 음성 인식 기반 인터페이스**  
  플레이어는 음성 명령을 통해 게임 내 설정을 조정하며, 창의적인 상호작용이 가능합니다.
  
- **👾 커스텀 몬스터 생성**  
  플레이어가 입력한 텍스트를 기반으로 AI가 고유한 몬스터를 생성하여 게임에 등장시킵니다.
  
- **👫 협동 플레이 및 웨이브 전투**  
  플레이어는 팀을 이루어 웨이브 단위로 생성되는 몬스터를 협력하여 처치하고, 보스 몬스터와의 전투에서 승리해야 합니다.

## 👥 팀 구성

| **AI** | **Backend** | **Unreal** | **Unreal** |
| :------: |  :------: | :------: | :------: |
[<img src="https://avatars.githubusercontent.com/u/83965086?v=4" height=150 width=150> <br/> @fesua](https://github.com/fesua) |  [<img src="https://avatars.githubusercontent.com/u/113831848?v=4" height=150 width=150> <br/> @Munhangyeol](https://github.com/Munhangyeol) | [<img src="https://avatars.githubusercontent.com/u/100117286?v=4" height=150 width=150> <br/> @GameTithe](https://github.com/GameTithe) | [<img src="https://avatars.githubusercontent.com/u/138466881?v=4" height=150 width=150> <br/> @caprisunlike](https://github.com/caprisunlike) |


## 💻 개발 환경
- **운영체제**: Windows 11, macOS
- **IDE**: Unreal Engine, IntelliJ
- **프레임워크 및 라이브러리**: Spring, TensorFlow
- **프로그래밍 언어**: Java, Python, SQL
- **버전 관리**: GitHub
- **협업 도구**: Notion, Discord, Google Meet
- **서버 및 데이터베이스**: AWS EC2, RDS, Docker
- **CI/CD 파이프라인**: GitHub Actions


## 🔧 기술 및 브랜치 전략
### 사용 기술
마음 AI의 STT API 기반의 음성 인식과 텍스트-3D 변환 기술을 통해 커스텀 몬스터 생성과 실시간 협동 플레이를 구현하였습니다.

### 브랜치 전략
효율적인 코드 관리를 위해 GitHub 브랜치 전략을 채택하여 협업을 원활히 진행합니다.

## 🧩 주요 로직
- **로그인 및 회원가입**  
  JWT를 통한 사용자 인증 및 보안 강화 로직 설계

- **로비 및 대기방 시스템**  
  Host와 Join을 통해 유저가 게임 방을 생성하거나 참가하며, 모든 인원이 모이면 자동으로 게임이 시작됩니다.

- **커스텀 몬스터 생성**  
  Host 유저가 입력한 텍스트를 바탕으로 AI가 커스텀 보스 몬스터를 생성하여 게임 내 소환합니다.

- **웨이브 진행 및 보스 전투**  
  웨이브 단위로 몬스터가 등장하고, 5개 웨이브 이후 보스와의 전투가 시작됩니다.

## 🛠 문제 해결 과정


## 💡 통찰 및 성찰

