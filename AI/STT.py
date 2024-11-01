import base64
import requests
class STTAPI:
    def __init__(self):
        self.base_url = ""
        self.headers = {
            'Content-Type': 'application/json',
            'cache-control': 'no-cache'
        }
    def convert_audio_to_text(self, wav_file_path):
        # WAV 파일을 base64로 변환
        with open(wav_file_path, 'rb') as audio_file:
            audio_content = audio_file.read()
            base64_audio = base64.b64encode(audio_content).decode('utf-8')
        # API 요청 데이터 구성
        request_data = {
            "app_id": "",
            "name": "",
            "item": [""],
            "param": [base64_audio]
        }
        try:
            # 디버깅을 위해 실제 요청 데이터 출력 (base64 문자열은 길이가 길어서 생략)
            debug_data = request_data.copy()
            debug_data['param'] = ['base64 string']
            print("Sending request with data:", debug_data)
            # API 호출
            response = requests.post(
                self.base_url,
                headers=self.headers,
                json=request_data
            )
            print("Response status code:", response.status_code)
            print("Response content:", response.text)
            response.raise_for_status()
            result = response.json()
            if '답변' in result:
                return result['답변']
            return result
        except requests.exceptions.RequestException as e:
            print(f"Error during API request: {str(e)}")
            return None
def main():
    stt = STTAPI()
    wav_file = r""
    result = stt.convert_audio_to_text(wav_file)
    if result:
        print("변환된 텍스트:", result)
    else:
        print("변환에 실패했습니다.")
if __name__ == "__main__":
    main()
